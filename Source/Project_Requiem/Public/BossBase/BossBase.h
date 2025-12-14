// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BossBase.generated.h"

class ABossProjectile;


// 보스 상태
UENUM(BlueprintType)
enum class EBossState : uint8
{
	Idle			UMETA(DisplayName = "Idle"),
	Chase			UMETA(DisplayName = "Chase"),
	Attack			UMETA(DisplayName = "Attack"),
	PhaseChange 	UMETA(DisplayName = "PhaseChange"),
	Dead        	UMETA(DisplayName = "Dead"),
};

// 보스 공격 타입
UENUM(BlueprintType)
enum class EBossAttackType : uint8
{
	Melee	UMETA(DisplayName = "Melee"),
	Ranged	UMETA(DisplayName = "Ranged")
};


// 보스가 데미지를 받았을 때
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnBossDamaged,
	float, CurrentHP,
	float, MaxHP
);

// 보스가 죽었을 때
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnBossDead,
	float, EXP
);

// 상태 변경
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnBossStateChanged,
	EBossState, NewState,
	EBossState, OldState
);

// 페이즈 변경
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnBossPhaseChanged,
	int32, NewPhase,
	int32, OldPhase
);


UCLASS()
class PROJECT_REQUIEM_API ABossBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABossBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 보스 상태
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|State")
	EBossState CurrentState = EBossState::Idle;

	// 전투 시작하자마자 자동으로 Chase 상태로 갈지 여부
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|State")
	bool bAutoStartChase = true;

	// 상태 전환용 함수
	void SetBossState(EBossState NewState);

	// 상태별 업데이트 함수
	void UpdateState(float DeltaTime);
	void UpdateIdle(float DeltaTime);
	void UpdateChase(float DeltaTime);
	void UpdateAttack(float DeltaTime);
	void UpdatePhaseChange(float DeltaTime);
	void UpdateDead(float DeltaTime);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// 추적할 타겟
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Target")
	ACharacter* TargetCharacter = nullptr;

	// 보스 이동 속도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Movement")
	float WalkSpeed = 400.0f;

	// 플레이어랑 겹치는거 방지용 최소 거리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Movement")
	float StoppingDistance = 80.0f;

	// 보스 체력
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Status")
	float MaxHP = 1000.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Status")
	float CurrentHP = 0.0f;

	// 죽었을 때 줄 경험치
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Reward")
	float EXP = 500.0f;

	// 근접 공격 데미지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Attack")
	float MeleeDamage = 30.0f;

	// 근접 공격 범위
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Attack")
	float MeleeAttackRange = 200.0f;

	// 쿨타임
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Attack")
	float MeleeAttackInterval = 1.5f;

	float TimeSinceLastMeleeAttack = 0.0f;

	// 원거리 공격 사용 여부
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Attack|Ranged")
	bool bUseRangedAttack = false;

	// 원거리 공격 가능한 최대 거리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Attack|Ranged")
	float RangedAttackRange = 800.0f;

	// 원거리 공격 쿨타임
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Attack|Ranged")
	float RangedAttackInterval = 3.0f;

	float TimeSinceLastRangedAttack = 0.0f;

	// 발사체 클래스
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Attack|Ranged")
	TSubclassOf<ABossProjectile> RangedProjectileClass;

	// 원거리 공격 데미지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Attack|Ranged")
	float RangedDamage = 20.0f;

	// 발사 함수
	void PerformRangedAttack();


	// 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Boss|Event")
	FOnBossDamaged OnBossDamaged;

	UPROPERTY(BlueprintAssignable, Category = "Boss|Event")
	FOnBossDead OnBossDead;

	UPROPERTY(BlueprintAssignable, Category = "Boss|Event")
	FOnBossStateChanged OnBossStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "Boss|Event")
	FOnBossPhaseChanged OnBossPhaseChanged;

	// 플레이어 공격 함수
	void PerformMeleeAttack();

public:
	// ApplyDamage로 들어오는 데미지 처리
	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser
		) override;

protected:
	// 페이즈 관련

	// 페이즈 시스템 사용 여부
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Phase")
	bool bUsePhaseSystem = false;

	// 현재 페이즈
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Phase")
	int32 CurrentPhase = 1;

	// 2페이즈 시작 HP비율
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Phase", meta = (ClampMin = "0.0", ClampMax = "1.0", EditCondition = "bUsePhaseSystem"))
	float Phase2StartHPRatio = 0.5f;

	// 2페이즈 스탯 배수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Phase", meta = (EditCondition = "bUsePhaseSystem"))
	float Phase2_WalkSpeedMultiplier = 1.2f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Phase", meta = (EditCondition = "bUsePhaseSystem"))
	float Phase2_MeleeDamageMultiplier = 1.5f;

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Boss|Phase")
	void OnPhaseChanged(int32 NewPhase, int32 OldPhase);
	virtual void OnPhaseChanged_Implementation(int32 NewPhase, int32 OldPhase);

	UFUNCTION(BlueprintCallable, Category = "Boss|Phase")
	void FinishPhaseChange();

	UFUNCTION(BlueprintCallable, Category = "Boss|Attack")
	void ApplyMeleeDamage();

// 애니메이션 몽타주
protected:
	// 근접 공격 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Anim")
	UAnimMontage* MeleeAttackMontage = nullptr;

	// 2페이즈 시작 연출 몽타주 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Anim")
	UAnimMontage* PhaseChangeMontage = nullptr;

	// 죽을 때 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Anim")
	UAnimMontage* DeathMontage = nullptr;

};
