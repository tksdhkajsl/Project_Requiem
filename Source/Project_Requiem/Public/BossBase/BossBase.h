// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BossBase.generated.h"

class ABossProjectile; // 전방 선언(헤더 include 줄여서 컴파일 속도/의존성 감소)

// =========================
// 보스 상태(Enum)
// =========================
UENUM(BlueprintType)
enum class EBossState : uint8
{
	Idle        UMETA(DisplayName = "Idle"),        // 대기(등장/휴식/아무것도 안 함)
	Chase       UMETA(DisplayName = "Chase"),       // 추적(타겟 쫓아감)
	Attack      UMETA(DisplayName = "Attack"),      // 공격(근접/원거리 쿨타임 돌리며 공격)
	PhaseChange UMETA(DisplayName = "PhaseChange"), // 페이즈 전환 연출/잠깐 멈춤 상태
	Dead        UMETA(DisplayName = "Dead"),        // 사망 상태
};

// =========================
// 보스 공격 타입(Enum) - (현재는 “구분값” 용도)
// =========================
UENUM(BlueprintType)
enum class EBossAttackType : uint8
{
	Melee  UMETA(DisplayName = "Melee"),  // 근접
	Ranged UMETA(DisplayName = "Ranged")  // 원거리
};

// =========================
// 델리게이트(이벤트 브로드캐스트용)
// - BP에서도 바인딩 가능하도록 Dynamic Multicast 사용
// =========================

// 보스가 데미지를 받았을 때: (CurrentHP, MaxHP)만 전달
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnBossDamaged,
	float, CurrentHP,
	float, MaxHP
);

// 보스가 죽었을 때: EXP 전달
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnBossDead,
	float, EXP
);

// 상태 변경: NewState, OldState 전달
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnBossStateChanged,
	EBossState, NewState,
	EBossState, OldState
);

// 페이즈 변경: NewPhase, OldPhase 전달
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnBossPhaseChanged,
	int32, NewPhase,
	int32, OldPhase
);

// =========================
// 보스 베이스 클래스
// =========================
UCLASS()
class PROJECT_REQUIEM_API ABossBase : public ACharacter
{
	GENERATED_BODY()

public:
	ABossBase(); // 생성자: 기본값/컴포넌트 세팅/초기 속도 세팅 등

protected:
	virtual void BeginPlay() override; // 스폰/게임 시작 시 1회 실행

	// -------------------------
	// 상태 관련
	// -------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|State")
	EBossState CurrentState = EBossState::Idle; // 현재 FSM 상태

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|State")
	bool bAutoStartChase = true; // 시작하자마자 추적할지(플레이어가 있으면)

	void SetBossState(EBossState NewState); // 상태 바꾸는 “유일한” 통로(권장)

	void UpdateState(float DeltaTime);     // 매 Tick마다 CurrentState에 맞는 Update로 분기
	void UpdateIdle(float DeltaTime);
	void UpdateChase(float DeltaTime);
	void UpdateAttack(float DeltaTime);
	void UpdatePhaseChange(float DeltaTime);
	void UpdateDead(float DeltaTime);

public:
	virtual void Tick(float DeltaTime) override; // 매 프레임 실행(쿨타임 누적/상태 업데이트)

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// 보스는 보통 입력 없음(플레이어용) -> 그래도 Character 기본 구조상 override 해둠

protected:
	// -------------------------
	// 정보/타겟/이동
	// -------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Info")
	FText BossName; // “이름용 FText 변수” (UI 표기 용도)

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Target")
	ACharacter* TargetCharacter = nullptr; // 추적/공격 타겟(플레이어 캐시)

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Movement")
	float WalkSpeed = 400.0f; // 이동속도(Chase 시 CharacterMovement에 반영)

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Movement")
	float StoppingDistance = 80.0f; // 너무 가까우면 겹침 방지로 멈추는 거리

	// -------------------------
	// 체력/보상
	// -------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Status")
	float MaxHP = 1000.0f; // 최대 체력

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Status")
	float CurrentHP = 0.0f; // 현재 체력(BeginPlay에서 MaxHP로 초기화)

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Reward")
	float EXP = 500.0f; // 사망 시 지급 경험치

	// -------------------------
	// 근접 공격
	// -------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Attack")
	float MeleeDamage = 30.0f; // 근접 공격 데미지

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Attack")
	float MeleeAttackRange = 200.0f; // 근접 공격 “유효 거리”

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Attack")
	float MeleeAttackInterval = 1.5f; // 근접 공격 쿨타임

	float TimeSinceLastMeleeAttack = 0.0f; // Tick에서 누적해서 쿨타임 체크

	// -------------------------
	// 원거리 공격
	// -------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Attack|Ranged")
	bool bUseRangedAttack = false; // 원거리 사용 여부(페이즈2 진입 등에서 true 가능)

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Attack|Ranged")
	float RangedAttackRange = 800.0f; // 원거리 공격 거리(Attack 상태 유지/진입 판단에 사용)

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Attack|Ranged")
	float RangedAttackInterval = 3.0f; // 원거리 공격 쿨타임

	float TimeSinceLastRangedAttack = 0.0f; // Tick에서 누적

	// 발사 소켓(오른손/왼손)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Attack|Ranged")
	FName RightHandSocketName = TEXT("RightHandSlashSocket");
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Attack|Ranged")
	FName LeftHandSocketName = TEXT("LeftHandSlashSocket");

	// 발사체 클래스(스폰할 클래스)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Attack|Ranged")
	TSubclassOf<ABossProjectile> RangedProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Attack|Ranged")
	float RangedDamage = 20.0f; // 발사체 데미지(InitProjectile로 전달)

	void PerformRangedAttack(); // “원거리 공격 시도” (몽타주 재생 or 즉시 발사)

	// -------------------------
	// 델리게이트(외부 UI/연출에서 바인딩)
	// -------------------------
	UPROPERTY(BlueprintAssignable, Category = "Boss|Event")
	FOnBossDamaged OnBossDamaged;

	UPROPERTY(BlueprintAssignable, Category = "Boss|Event")
	FOnBossDead OnBossDead;

	UPROPERTY(BlueprintAssignable, Category = "Boss|Event")
	FOnBossStateChanged OnBossStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "Boss|Event")
	FOnBossPhaseChanged OnBossPhaseChanged;

	// 근접 공격 “시도”(몽타주 재생). 실제 데미지는 AnimNotify에서 ApplyMeleeDamage() 호출
	void PerformMeleeAttack();

public:
	// ApplyDamage로 들어오는 데미지 처리(체력 감소/죽음/페이즈 전환)
	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser
	) override;

protected:
	// -------------------------
	// 페이즈 시스템
	// -------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Phase")
	bool bUsePhaseSystem = false; // 페이즈 시스템 사용 여부(보스별로 켜고 끔)

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Phase")
	int32 CurrentPhase = 1; // 현재 페이즈(기본 1)

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Phase",
		meta = (ClampMin = "0.0", ClampMax = "1.0", EditCondition = "bUsePhaseSystem"))
	float Phase2StartHPRatio = 0.5f; // HP 비율이 이 값 이하이면 2페이즈 진입

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Phase", meta = (EditCondition = "bUsePhaseSystem"))
	float Phase2_WalkSpeedMultiplier = 1.2f; // 2페이즈 이동속도 배수

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Phase", meta = (EditCondition = "bUsePhaseSystem"))
	float Phase2_MeleeDamageMultiplier = 1.5f; // 2페이즈 근접데미지 배수

public:
	// 페이즈 변경 시 “훅”
	UFUNCTION(BlueprintNativeEvent, Category = "Boss|Phase")
	void OnPhaseChanged(int32 NewPhase, int32 OldPhase);
	virtual void OnPhaseChanged_Implementation(int32 NewPhase, int32 OldPhase);

	// 페이즈 연출이 끝났을 때(몽타주 끝 노티파이/타이밍에서 호출)
	UFUNCTION(BlueprintCallable, Category = "Boss|Phase")
	void FinishPhaseChange();

	// 애님 노티파이에서 호출할 실제 데미지 적용 함수
	UFUNCTION(BlueprintCallable, Category = "Boss|Attack")
	void ApplyMeleeDamage();

	UFUNCTION(BlueprintCallable, Category = "Boss|Attack")
	void ApplyRangedAttack(); // 기본 오른손 발사 같은 “간단 래퍼” 용도

	UFUNCTION(BlueprintCallable, Category = "Boss|Attack")
	void ApplyRangedAttackFromSocket(FName SocketName); // 소켓 지정 발사(양손 패턴에 핵심)

	// -------------------------
	// 애니메이션 몽타주 포인터들
	// -------------------------
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Anim")
	UAnimMontage* MeleeAttackMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Anim")
	UAnimMontage* PhaseChangeMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Anim")
	UAnimMontage* DeathMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Anim")
	UAnimMontage* RangedAttackMontage = nullptr;
};
