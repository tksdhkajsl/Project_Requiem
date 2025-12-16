// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Stats/Data/EFullStats.h"
#include "EnemyCharacter.generated.h"

class UAnimMontage;

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
class AEnemyCharacter : public ABaseCharacter
{
	GENERATED_BODY()
	
public:
    AEnemyCharacter();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    // =================================================================
    // 1. 피격 및 사망 (BaseCharacter 오버라이드)
    // =================================================================
    /** * BaseCharacter의 TakeDamage를 오버라이드하여
     * LastHitInstigator 갱신 및 AI 어그로 처리를 수행.
     */
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

    /** * 사망 처리: BaseCharacter.h의 주석에 따라
     * Destroy 대신 Hidden/NoCollision 처리를 수행.
     */
    virtual void Die() override;

    /** 적 처치 시 플레이어에게 줄 (재화)경험치 양 */
    UPROPERTY(EditAnywhere, Category = "Stats")
    float ExpReward = 10.0f;

    // =================================================================
    // 2. 리스폰 시스템 (Bonfire 연동)
    // =================================================================
    /** 화톳불(안전지대/상점) 휴식 시 적을 초기 상태로 되돌리는 함수 */
    void ResetEnemy();

    // =================================================================
    // 3. AI 및 배회 (Navigation)
    // =================================================================
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|State")
    EEnemyState CurrentState = EEnemyState::Patrol;

    /** 스폰 위치 저장 (배회 중심점) */
    FVector HomeLocation;

    /** 배회 반경 */
    UPROPERTY(EditAnywhere, Category = "AI|Patrol")
    float PatrolRadius = 600.0f;

    /** 플레이어 추격 시작 거리 */
    UPROPERTY(EditAnywhere, Category = "AI|Chase")
    float DetectionRange = 800.0f;

    /** AI 상태 업데이트 로직 */
    void UpdateAIState();
    void MoveToRandomPatrolPoint();

    // =================================================================
    // 4. 공격 (Combat)
    // =================================================================
    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    UAnimMontage* AttackMontage;

    UPROPERTY(EditAnywhere, Category = "Combat")
    float AttackRange = 150.0f;

    UPROPERTY(EditAnywhere, Category = "Combat")
    float AttackDelay = 2.0f;

    FTimerHandle AttackTimerHandle;

    /** 공격 시도 (AI 호출) */
    void PerformAttack();

    /** 공격력 가져오기 */
    float GetAttackDamage() const;
};
