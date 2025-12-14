// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "LastBossCharacter.generated.h"

// 플레이어에게 보낼 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnApplyDamage, float, DamageAmount);	// 데미지
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnApplyExp, float, ExpAmount);			// 경험치
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLastBossName, FText, BossName);		// 이름

/**
 * 
 */
UCLASS()
class PROJECT_REQUIEM_API ALastBossCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	ALastBossCharacter();

	virtual void BeginPlay() override;

public:
	// 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FOnApplyDamage OnApplyDamage;
	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FOnApplyExp OnApplyExp;
	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FOnLastBossName OnLastBossName;

public:
	// 이동 위치
	virtual void Move(const FVector& Direction, float Value) override;

	// 데미지를 줄 때
	virtual void ApplyDamage(float DamageAmount) override;

	// 데미지를 받을 때
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	// 죽었을 때
	virtual void Die() override;

	// 경험치 줄 때
	void ApplyExp(float ExpAmount);

	// Getter
	// 투사체 스폰 위치 외부 접근용
	UFUNCTION(BlueprintCallable)
	inline class USceneComponent* GetSceneComponent() { return SpawnProjectileLocation.Get(); }



protected:
	// 보스 패턴 별 몽타주 추가 함수
	void AddPatternMontage();

	// 보스의 패턴 재생 함수
	void RandomPatternPlay();

protected:
	// 투사체 스폰 위치
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<class USceneComponent> SpawnProjectileLocation = nullptr;;

	// 보스 상태에 따라 실행될 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<class UAnimMontage> SpawnMontage = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<class UAnimMontage> DieMontage = nullptr;

	// 보스 1페이즈 패턴 때 실행될 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage|Phase 1")
	TObjectPtr<class UAnimMontage> PhaseOneMontage1 = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage|Phase 1")
	TObjectPtr<class UAnimMontage> PhaseOneMontage2 = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage|Phase 1")
	TObjectPtr<class UAnimMontage> PhaseOneMontage3 = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage|Phase 1")
	TObjectPtr<class UAnimMontage> PhaseOneMontage4 = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage|Phase 1")
	TObjectPtr<class UAnimMontage> PhaseOneMontage5 = nullptr;
	
	// 보스 2페이즈 패턴 때 실행될 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage|Phase 2")
	TObjectPtr<class UAnimMontage> PhaseTwoMontage1 = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage|Phase 2")
	TObjectPtr<class UAnimMontage> PhaseTwoMontage2 = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage|Phase 2")
	TObjectPtr<class UAnimMontage> PhaseTwoMontage3 = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage|Phase 2")
	TObjectPtr<class UAnimMontage> PhaseTwoMontage4 = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage|Phase 2")
	TObjectPtr<class UAnimMontage> PhaseTwoMontage5 = nullptr;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "00_Setting|Exp", meta = (ClampMin = "0"))
	float DropExp = 0.0f;

	UPROPERTY(VisibleAnywhere, Category = "Information|Phase")
	int32 Phase = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Information|Name")
	FText BossName;

private:
	TArray<TObjectPtr<class UAnimMontage>> PhaseOnePatterns;
	TArray<TObjectPtr<class UAnimMontage>> PhaseTwoPatterns;

	float MinHp = 0.0f;
};
