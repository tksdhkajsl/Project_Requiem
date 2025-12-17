#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class UStatComponent;
struct FOffspringData;

/*
공격자: Player / Enemy 공통 패턴
void APlayerCharacter::Attack1()
{
    // 해당 공격 애니메이션 재생

    FHitResult HitResult;

    const FVector Start = GetActorLocation();
    const FVector End = Start + GetActorForwardVector() * 150.f;

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    const bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult,
        Start,
        End,
        ECC_Pawn,
        Params
    );

    if (!bHit) return;

    AActor* HitActor = HitResult.GetActor();
    if (!IsValid(HitActor)) return;

    // 데미지는 엔진 파이프라인에 태운다
    Attack(HitActor, 해당 공격 데미지);
}


죽었을 때 입었을때(플레이어)
void APlayerCharacter::Die() 
{ 
    // 죽음 애니메이션 재생
    PlayDeathAnimation();

    // 입력 차단
    DisableInput(nullptr);

    // 화롯불(체크포인트)에서 리스폰
    RespawnAtBonfire();
}
void AEnemyCharacter::Die()
{
    // 일반 몹 리스폰 시 
       1. hiddeningame(false) + ECC::colision 설정 필요
       2. bIseDead = false;

    if (bIsDead) return;
    bIsDead = true;

    // 1. 막타 친 플레이어 찾기
    if (LastHitInstigator.IsValid()) {
        APawn* KillerPawn = LastHitInstigator->GetPawn();
        APlayerCharacter* Player = Cast<APlayerCharacter>(KillerPawn);

        if (Player) {
            Player->AddExp(ExpReward);
        }
    }

    // 2. 사망 처리
    Super::Die();

    // 보스면 Destroy, 적 일반몹은 hiddeningame + ECC::Nocolision
}
*/

UCLASS()
class PROJECT_REQUIEM_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()
	
#pragma region 기본 생성
public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
protected:
	virtual void BeginPlay() override;
#pragma endregion

#pragma region 스탯 컴포넌트 관련
public:
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "00_StatCompoent")
    FText MonsterName;

	FORCEINLINE UStatComponent* GetStatComponent() const { return StatComponent; }

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="00_StatCompoent")
	int32 LevelUpPoint = 5;

	/** @brief 캐릭터의 스탯을 관리하는 컴포넌트 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStatComponent* StatComponent;
#pragma endregion

#pragma region 캐릭터 기본 함수들
public:
    /**
     * @brief 피해 적용
     * @param DamageAmount 적용할 피해량
     */
    UFUNCTION(BlueprintCallable, Category = "Character")
    virtual void ReceiveDamage(float DamageAmount);
    /**
     * @brief 회복 적용
     * @param HealAmount 회복량
     */
    UFUNCTION(BlueprintCallable, Category = "Character")
    virtual void ApplyHeal(float HealAmount);
    /** @brief 캐릭터 사망 처리 */
    UFUNCTION(BlueprintCallable, Category = "Character")
    virtual void Die();
    /**
     * @brief 캐릭터 경험치 처리
     * @param Amount 경험치량(-도 가능)
     */
    UFUNCTION(BlueprintCallable, Category = "Character")
    virtual void AddExp(float Amount);
    // ---- Movement ----
    virtual void Move(const FVector& Direction, float Value);
    virtual void StartSprint();
    virtual bool CanSprint() const;
    virtual void StopSprint();
    virtual void Jump() override;
    virtual void StopJumping() override;
    virtual void StopMovement();

    // ---- Combat & Actions ----
    UFUNCTION(BlueprintCallable) /// 수정사항
    virtual void Attack(AActor* HitActor, float AttackDamage);
    virtual void StopAttack();
    virtual bool CanAttack() const;
    virtual void Interact();

    // ---- Damage & Death ----
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
    virtual void OnDeathFinished();

    // ---- Status & CC ----
    virtual void SetStunned(bool bStunned);
    virtual bool IsStunned() const;
#pragma endregion

protected:
    UPROPERTY()
    TWeakObjectPtr<AController> LastHitInstigator;
};
