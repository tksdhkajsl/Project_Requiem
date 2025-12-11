#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class UStatComponent;

struct FOffspringData;

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
    virtual void ApplyDamage(float DamageAmount);
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
    virtual void Attack();
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
};
