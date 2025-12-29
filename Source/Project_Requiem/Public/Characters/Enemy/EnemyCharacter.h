#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h" //
#include "Stats/Data/EFullStats.h"    //
#include "EnemyCharacter.generated.h"

class UAnimMontage;
class UWidgetComponent;
class UEnemyCharacterHPWidget;

/** 적 AI 상태 정의 */
UENUM(BlueprintType)
enum class EEnemyState : uint8
{
    Idle,
    Patrol, // 배회
    Chase,  // 추격
    Attack, // 공격
    Dead    // 사망
};

UCLASS()
class PROJECT_REQUIEM_API AEnemyCharacter : public ABaseCharacter
{
    GENERATED_BODY()

    public:
    AEnemyCharacter();

    // HP 위젯 컴포넌트
    UPROPERTY(VisibleAnywhere, Category = "UI")
    UWidgetComponent* HPWidgetComponent;

    // 실제 위젯 클래스 포인터
    UPROPERTY()
    UEnemyCharacterHPWidget* HPWidget;

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    // =================================================================
    // 1. 피격 및 사망 (BaseCharacter 오버라이드)
    // =================================================================
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

    /** 사망 처리 (래그돌 실행) */
    virtual void Die() override;

    /** 시체 숨김용 타이머 핸들 */
    FTimerHandle DeadTimerHandle;

    /** 시체를 숨기고 물리를 끄는 함수 (타이머 호출용) */
    void HideBody();

    /** 적 처치 시 플레이어에게 줄 경험치 양 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float ExpReward = 10.0f;

    //사망 후 레그돌 대기시간(이후 안보임처리)
    float RagdollWaitTime = 5.0f;

    // =================================================================
    // 2. 리스폰 시스템 (Bonfire 연동)
    // =================================================================
    /** 적을 초기 상태로 되돌리는 함수 (위치, HP, 물리, AI 복구) */
    void ResetEnemy();

    // =================================================================
    // 3. AI 및 배회 (Navigation)
    // =================================================================
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|State")
    EEnemyState CurrentState = EEnemyState::Patrol;

    /** 스폰 위치 저장 (배회 중심점) */
    FVector HomeLocation;
    /** 스폰 회전 저장 (리스폰 시 원상복구용) */
    FRotator HomeRotation;
    // 플레이어 발견 시 재생할 소리 (하울링, 경고음 등)
    UPROPERTY(EditAnywhere, Category = "Audio")
    USoundBase* DetectSound;

    /** 배회 반경 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Patrol")
    float PatrolRadius = 600.0f;

    /** 플레이어 추격 시작 거리 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Chase")
    float DetectionRange = 1500.0f;

    // 플레이어 추격 가능 거리
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Chase")
    float CanChaseRange = 8000.0f;

    /** 패트롤 대기 시간용 타이머 핸들 */
    FTimerHandle PatrolTimerHandle;

    void UpdateAIState();
    void MoveToRandomPatrolPoint();

    // =================================================================
    // 4. 공격 (Combat)
    // =================================================================
    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    UAnimMontage* AttackMontage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float AttackRange = 80.0f; // 공격 사거리

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float AttackDelay = 1.5f;   // 공격 쿨타임

    FTimerHandle AttackTimerHandle;

    /** 공격 애니메이션 재생 명령 (데미지 X) */
    void PerformAttack();

    /** 실제 타격 판정 (애니메이션 노티파이에서 호출) */
    UFUNCTION(BlueprintCallable)
    virtual void AttackHitCheck();

    // 근접 공격 성공 시 재생할 타격음
    UPROPERTY(EditAnywhere, Category = "Combat")
    USoundBase* MeleeHitSound;

    float GetAttackDamage() const;
};