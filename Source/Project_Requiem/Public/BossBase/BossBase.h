// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"

#include "BossBase.generated.h"


class ABossProjectile;
class UAnimMontage;

//보스 상태
UENUM(BlueprintType)
enum class EBossState : uint8
{
	Idle        UMETA(DisplayName = "Idle"),        
	Chase       UMETA(DisplayName = "Chase"),       
	Attack      UMETA(DisplayName = "Attack"),      
	PhaseChange UMETA(DisplayName = "PhaseChange"), 
	Dead        UMETA(DisplayName = "Dead"),        
};


// 보스 공격 타입
UENUM(BlueprintType)
enum class EBossAttackType : uint8
{
	Melee  UMETA(DisplayName = "Melee"),  
	Ranged UMETA(DisplayName = "Ranged")  
};

// 보스 패턴
UENUM(BlueprintType)
enum class EBossPattern : uint8
{
	None UMETA(DisplayName = "None"),

	// 패턴
	Pattern1  UMETA(DisplayName = "Pattern1 "),
	Pattern2  UMETA(DisplayName = "Pattern2 "),
	Pattern3  UMETA(DisplayName = "Pattern3 "),
	Pattern4  UMETA(DisplayName = "Pattern4 "),
	Pattern5  UMETA(DisplayName = "Pattern5 "),
	Pattern6  UMETA(DisplayName = "Pattern6 "),
	Pattern7  UMETA(DisplayName = "Pattern7 "),
	Pattern8  UMETA(DisplayName = "Pattern8 "),

	// 페이즈2 전용
	Pattern9  UMETA(DisplayName = "Pattern9 "),
	Pattern10 UMETA(DisplayName = "Pattern10 "),
	Pattern11 UMETA(DisplayName = "Pattern11 "),
	Pattern12 UMETA(DisplayName = "Pattern12 "),
	Pattern13 UMETA(DisplayName = "Pattern13 "),
	Pattern14 UMETA(DisplayName = "Pattern14 "),
	Pattern15 UMETA(DisplayName = "Pattern15 "),
	Pattern16 UMETA(DisplayName = "Pattern16 "),
};

	




// 보스가 데미지를 받았을 때
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnBossDamaged,
	float, CurrentHP,
	float, MaxHP
);

// 보스가 죽었을 때 (EXP)
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

USTRUCT(BlueprintType)
struct FBossPatternData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EBossPattern Pattern = EBossPattern::None;

	// 실행 거리 조건
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MinRange = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxRange = 999999.0f;

	// 페이즈 조건 (0이면 무조건 허용, 1/2면 해당 페이즈에서 가능)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 OnlyPhase = 0;

	// 선택 가중치
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Weight = 1.0f;

	// 패턴 자체 쿨타임(초)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Cooldown = 2.0f;

	// 실행 몽타주(없으면 즉시 로직 실행)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* Montage = nullptr;

	// 보스 주변 범위 공격(AoE)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AoERadius = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AoEDamage = 0.0f;

	// 무적 지속시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float InvulnerableDuration = 0.0f;
	
};


// 보스 베이스 클래스
UCLASS()
class PROJECT_REQUIEM_API ABossBase : public ACharacter
{
	GENERATED_BODY()

public:
	ABossBase();

protected:
	virtual void BeginPlay() override;

	
	// 상태 관련
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|State")
	EBossState CurrentState = EBossState::Idle; 

	// 시작하자마자 추적할지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|State")
	bool bAutoStartChase = true; 

	void SetBossState(EBossState NewState);

	void UpdateState(float DeltaTime);     
	void UpdateIdle(float DeltaTime);
	void UpdateChase(float DeltaTime);
	void UpdateAttack(float DeltaTime);
	void UpdatePhaseChange(float DeltaTime);
	void UpdateDead(float DeltaTime);

public:
	virtual void Tick(float DeltaTime) override; 

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	

protected:
	
	// 보스 이름
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Info")
	FText BossName; 

	// 공격 타겟(플레이어 캐시)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Target")
	ACharacter* TargetCharacter = nullptr; 

	// 이동속도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Movement")
	float WalkSpeed = 400.0f; 

	// 너무 가까우면 겹침 방지로 멈추는 거리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Movement")
	float StoppingDistance = 80.0f; 

	// 최대 체력
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Status")
	float MaxHP = 1000.0f;

	// 현재 체력
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Status")
	float CurrentHP = 0.0f; 

	// 사망 시 지급 경험치
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Reward")
	float EXP = 500.0f; 

	// 근접 공격 데미지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Attack")
	float MeleeDamage = 30.0f; 

	// 근접 공격 유효 거리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Attack")
	float MeleeAttackRange = 250.0f; 

	// 근접 공격 쿨타임
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Attack")
	float MeleeAttackInterval = 1.5f; 

	// 공격 중 이동 잠금
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Movement")
	bool bMovementLocked = false;

	void LockMovement();
	void UnlockMovement();

	// 쿨타임 체크
	float TimeSinceLastMeleeAttack = 0.0f;

	// 원거리 사용 여부
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Attack|Ranged")
	bool bUseRangedAttack = false; 

	// 원거리 공격 거리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Attack|Ranged")
	float RangedAttackRange = 800.0f; 

	// 원거리 공격 쿨타임
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Attack|Ranged")
	float RangedAttackInterval = 3.0f; 

	float TimeSinceLastRangedAttack = 0.0f;

	// 보스 공격 범위 튜닝
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Attack|Tuning")
	float MeleeZoneMaxRange = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Attack|Tuning")
	float RangedZoneMinRange = 700.0f;



	// 발사 소켓(오른손/왼손)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Attack|Ranged")
	FName RightHandSocketName = TEXT("RightHandSlashSocket");
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Attack|Ranged")
	FName LeftHandSocketName = TEXT("LeftHandSlashSocket");

	// 발사체 클래스
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Attack|Ranged")
	TSubclassOf<ABossProjectile> RangedProjectileClass;

	// 발사체 데미지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Attack|Ranged")
	float RangedDamage = 20.0f; 

	// 원거리 공격 시도
	void PerformRangedAttack(); 

	// 1페이즈 발사체
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Attack|Ranged")
	TSubclassOf<ABossProjectile> RangedProjectileClass_Phase1;

	// 2페이즈 발사체(VFX 다른버전)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Attack|Ranged")
	TSubclassOf<ABossProjectile> RangedProjectileClass_Phase2;

	// 2페이즈 멀티샷 옵션
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Attack|Ranged|Phase2")
	int32 Phase2_RangedShotCount = 3;

	// 좌우 퍼짐 각도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Attack|Ranged|Phase2")
	float Phase2_RangedSpreadYaw = 12.0f;

	// 멀티샷 사용 여부
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Attack|Ranged|Phase2")
	bool bPhase2_UseMultiShot = true;

	TSubclassOf<ABossProjectile> GetRangedProjectileClassByPhase() const;



	// 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Boss|Event")
	FOnBossDamaged OnBossDamaged;

	UPROPERTY(BlueprintAssignable, Category = "Boss|Event")
	FOnBossDead OnBossDead;

	UPROPERTY(BlueprintAssignable, Category = "Boss|Event")
	FOnBossStateChanged OnBossStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "Boss|Event")
	FOnBossPhaseChanged OnBossPhaseChanged;

	// 근접 공격 시도
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
	
	// 페이즈 시스템 사용 여부
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Phase")
	bool bUsePhaseSystem = false; 

	// 현재 페이즈
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Phase")
	int32 CurrentPhase = 1; 

	// HP 비율이 이 값 이하이면 2페이즈 진입
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Phase",
		meta = (ClampMin = "0.0", ClampMax = "1.0", EditCondition = "bUsePhaseSystem"))
	float Phase2StartHPRatio = 0.5f; 

	// 2페이즈 이동속도 배수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Phase", meta = (EditCondition = "bUsePhaseSystem"))
	float Phase2_WalkSpeedMultiplier = 1.2f; 

	// 2페이즈 근접데미지 배수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Phase", meta = (EditCondition = "bUsePhaseSystem"))
	float Phase2_MeleeDamageMultiplier = 1.5f; 

	// 보스 페이즈 시 범위 공격 계수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Phase|AOE")
	float PhaseChangeAoERadius = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Phase|AOE")
	float PhaseChangeAoEDamage = 40.0f;

public:
	
	UFUNCTION(BlueprintNativeEvent, Category = "Boss|Phase")
	void OnPhaseChanged(int32 NewPhase, int32 OldPhase);

	virtual void OnPhaseChanged_Implementation(int32 NewPhase, int32 OldPhase);

	// 페이즈 연출이 끝났을 때
	UFUNCTION(BlueprintCallable, Category = "Boss|Phase")
	void FinishPhaseChange();

	// 페이즈 전환 AoE 공격
	UFUNCTION(BlueprintCallable, Category = "Boss|Phase|AOE")
	void ApplyPhaseChangeAOE();

	// 애님 노티파이에서 호출할 실제 데미지 적용 함수
	UFUNCTION(BlueprintCallable, Category = "Boss|Attack")
	void ApplyMeleeDamage();

	UFUNCTION(BlueprintCallable, Category = "Boss|Attack")
	void ApplyRangedAttack();

	// 소켓 지정 발사
	UFUNCTION(BlueprintCallable, Category = "Boss|Attack")
	void ApplyRangedAttackFromSocket(FName SocketName); 

	UFUNCTION(BlueprintCallable, Category = "Boss|Invuln")
	void StartInvulnerability(float Duration);

	UFUNCTION(BlueprintCallable, Category = "Boss|Invuln")
	void EndInvulnerability();

	UFUNCTION(BlueprintCallable, Category = "Boss|Pattern")
	void ApplyCurrentPatternAOE();

	UFUNCTION(BlueprintCallable, Category = "Boss|Pattern")
	void StartCurrentPatternInvulnerability();

	// 원거리 몽타주 끝났을 때 이동 가능
	UFUNCTION()
	void OnRangedMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	// 근접 몽타주 종료
	UFUNCTION()
	void OnMeleeMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	// 데스 몽타주 종료
	UFUNCTION()
	void OnDeathMontageEnded(UAnimMontage* Montage, bool bInterrupted);

protected:
	// 애니메이션 몽타주 포인터
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Anim")
	UAnimMontage* MeleeAttackMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Anim")
	UAnimMontage* PhaseChangeMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Anim")
	UAnimMontage* DeathMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Anim")
	UAnimMontage* RangedAttackMontage = nullptr;

	// 피격 리액션
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Anim")
	UAnimMontage* HitReactMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|HitReact")
	bool bEnableHitReact = true;

	// 히트 리액션 쿨타임
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|HitReact", meta=(ClampMin = "0.0"))
	float HitReactCooldown = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|HitReact")
	bool bHitReactWhileExecutingPattern = false;

	float LastHitReactTime = -9999.0f;

	void TryPlayHitReact();

	// 히트 리액션 재생될때 현재 패턴을 끊을지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|HitReact")
	bool bInterruptPatternOnHitReact = true;

	// 끊을 때 블렌드아웃 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|HitReact", meta = (ClampMin = "0.0"))
	float HitReactInterruptBlendOut = 0.05f;

	// 페이즈 전환 직전에도 히트리액션 재생할지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|HitReact")
	bool bAllowHitReactBeforePhaseChange = true;

	// 무적 능력
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|HitReact")
	bool bIsInvulnerable = false;

	FTimerHandle InvulnerableTimerHandle;

	float PendingAoERadius = 0.0f;
	float PendingAoEDamage = 0.0f;
	float PendingInvulnerableDuration = 0.0f;


	// HitReact 진행 중인지
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|HitReact")
	bool bIsInHitReact = false;

	// 히트리액션이 끝난 뒤 플레이어 쉬는 타임
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|HitReact", meta = (ClampMin = "0.0"))
	float HitReactRecoveryTime = 0.7f;

	// 이 시간 전까지는 공격 금지
	float AttackBlockedUntilTime = 0.0f;

	//HitReact 중 보스가 추적도 멈출지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|HitReact")
	bool bStopChaseWhileHitReact = false;

	bool IsAttackBlocked() const;

	// 패턴 후딜 기본값
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Pattern", meta = (ClampMin = "0.0"))
	float DefaultPatternRecoveryTime = 0.5f;

	float PendingRecoveryTime = 0.0f;

public:

	// 패턴 종료 호출용(몽타주 끝/노티파이 끝에서 사용)
	UFUNCTION(BlueprintCallable, Category = "Boss|Pattern")
	void FinishCurrentPattern();

	// 히트 리액션 후 복귀 함수
	UFUNCTION()
	void OnHitReactMontageEnded(UAnimMontage* Montage, bool Interrupted);

protected:
	// 패턴 선택

	// 패턴 리스트
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Pattern")
	TArray<FBossPatternData> PatternTable;

	// 현재 실행 중 패턴
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Pattern")
	EBossPattern CurrentPattern = EBossPattern::None;

	// 패턴 실행 중인지 (공격 락 용도)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Pattern")
	bool bIsExecutingPattern = false;

	// 패턴별 마지막 사용 시간
	TMap<EBossPattern, float> LastPatternUseTime;



	// 패턴 함수

	// 후보 필터 후 선택
	EBossPattern SelectPattern(float DistanceToTarget) const;

	// 선택된 패턴 실행
	void ExecutePattern(EBossPattern Pattern);

	// 패턴 데이터 찾기
	const FBossPatternData* FindPatternData(EBossPattern Pattern) const;

	// 쿨타임 체크
	bool IsPatternOffCooldown(EBossPattern Pattern, float Now) const;

	// 패턴 강제 중단
	void AbortCurrentPatternForHitReact();
	

protected:

	// 보스 배경음악

	// 보스전 배경음악 에셋
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|BGM")
	USoundBase* BossBGMSound = nullptr;

	// 재생 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|BGM")
	UAudioComponent* BossBGMAudioComp = nullptr;

	// 재생 중인지
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|BGM")
	bool bBossBGMPlaying = false;

	// 페이즈별 보스전 배경음악

	// 1페이즈
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|BGM")
	USoundBase* BossBGM_Phase1 = nullptr;

	// 2페이즈
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|BGM")
	USoundBase* BossBGM_Phase2 = nullptr;

	// 페이즈 변경 시 자동으로 BGM 스위치할지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|BGM")
	bool bAutoSwitchBGMOnPhaseChanged = true;

	// 부드럽게 전환
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|BGM", meta = (ClampMin = "0.0"))
	float BGMFadeOutTime = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|BGM", meta = (ClampMin = "0.0"))
	float BGMFadeInTime = 0.5f;

public:

	// 보스 배경음악 관련 함수
	UFUNCTION(BlueprintCallable, Category = "Boss|BGM")
	void StartBossBGM();

	UFUNCTION(BlueprintCallable, Category = "Boss|BGM")
	void StopBossBGM();

	UFUNCTION(BlueprintCallable, Category = "Boss|BGM")
	void SetBossBGMSound(USoundBase* NewSound);

	UFUNCTION(BlueprintCallable, Category = "Boss|BGM")
	bool IsBossBGMPlaying() const { return bBossBGMPlaying; }

	UFUNCTION(BlueprintCallable, Category = "Boss|BGM")
	USoundBase* GetBossBGMSoundByPhase(int32 Phase) const;

	UFUNCTION(BlueprintCallable, Category = "Boss|BGM")
	void SwitchBossBGMByPhase(int32 Phase);


};
