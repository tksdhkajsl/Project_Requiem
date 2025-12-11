// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BossBase.generated.h"

UENUM(BlueprintType)
enum class EBossState : uint8
{
	Idle			UMETA(DisplayName = "Idle"),
	Chase			UMETA(DisplayName = "Chase"),
	Attack			UMETA(DisplayName = "Attack"),
	PhaseChange 	UMETA(DisplayName = "PhaseChange"),
	Dead        	UMETA(DisplayName = "Dead"),
};

// 보스가 데미지를 받았을 때
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FOnBossDamaged,
	float, CurrentHP,
	float, MaxHP,
	float, DamageAmount
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


	// 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Boss|Event")
	FOnBossDamaged OnBossDamaged;

	UPROPERTY(BlueprintAssignable, Category = "Boss|Event")
	FOnBossDead OnBossDead;

	UPROPERTY(BlueprintAssignable, Category = "Boss|Event")
	FOnBossStateChanged OnBossStateChanged;

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






};
