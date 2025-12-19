// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "LastBossCharacter.generated.h"

// 플레이어에게 보낼 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnApplyDamage, float, DamageAmount);	// 데미지
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnApplyExp, float, ExpAmount);			// 경험치
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLastBossName, FText, BossName);		// 이름

// 보스가 스폰되었을 경우 보낼 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLastBossSpawn);			// 보스가 스폰됨

// 보스가 죽었을 경우 보낼 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLastBossChangedPhase);	// 페이즈 변경
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLastBossDead);			// 보스 죽음

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

private:
	// 보스가 스폰할 때 실행하는 함수
	void LastBossSpawn();
	// 보스가 실행 끝났을 때 실행하는 함수
	void LastBossEndSpawn(UAnimMontage* Montage, bool bInterrupted);

public:
	// 델리게이트
	FOnApplyDamage OnApplyDamage;
	FOnApplyExp OnApplyExp;
	FOnLastBossName OnLastBossName;

	FOnLastBossSpawn OnLastBossSpawn;

	FOnLastBossChangedPhase OnLastBossChangedPhase;
	FOnLastBossDead	OnLastBossDead;


public:

	// 데미지를 받을 때(내 실제 피가 깍임)
	virtual void ReceiveDamage(float DamageAmount) final;

	// 데미지를 받을 때(엔진에서 적용됨)
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) final;

	// 죽었을 때
	virtual void Die() override;

	// 경험치 적용
	void ApplyExp(float ExpAmount);

	// Getter
	// 투사체 스폰 위치 외부 접근용
	UFUNCTION(BlueprintCallable)
	inline class USceneComponent* GetSceneComponent() { return SpawnProjectileLocation.Get(); }

	UFUNCTION(BlueprintCallable)
	inline int32 GetBossPhase() { return Phase; }

	inline bool IsPhaseChanged() { return bPhaseChanged; }

	inline const TArray<TObjectPtr<class UAnimMontage>> GetPhaseOnePatterns() const { return PhaseOnePatterns; }

	inline const TArray<TObjectPtr<class UAnimMontage>> GetPhaseTwoPatterns() const { return PhaseTwoPatterns; }


protected:
	// 보스 패턴 배열에 몽타주 추가 함수
	void AddPatternMontage();

protected:
	// 투사체 스폰 위치
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<class USceneComponent> SpawnProjectileLocation = nullptr;;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh")
	TObjectPtr<class USkeletalMesh> PhaseTwoSkeletalMesh = nullptr;

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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Montage|Patterns")
	TArray<TObjectPtr<class UAnimMontage>> PhaseOnePatterns;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Montage|Patterns")
	TArray<TObjectPtr<class UAnimMontage>> PhaseTwoPatterns;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "00_Setting|Exp", meta = (ClampMin = "0"))
	float DropExp = 0.0f;

	UPROPERTY(VisibleAnywhere, Category = "Information|Phase", meta = (ClampMin = "1", ClampMax = "2"))
	int32 Phase = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Information|Phase")
	bool bPhaseChanged = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Information|Name")
	FText BossName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Information")
	bool bLastBossInvincible = false;
private:
	float MinHp = 0.0f;
};
