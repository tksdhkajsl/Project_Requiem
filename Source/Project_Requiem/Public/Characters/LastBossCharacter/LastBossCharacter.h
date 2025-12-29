// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Interface/Boss/BossControlInterface.h"
#include "BossBase/BossBase.h"
#include "LastBossCharacter.generated.h"

// 플레이어에게 보낼 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnApplyExp, float, ExpAmount);	// 경험치

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLastBossSpawn);			// 보스가 스폰됨
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLastBossChangedPhase);	// 페이즈 변경
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLastBossEndChangedPhase);	// 페이즈 변경 종료
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLastBossStop);			// 보스 죽음

/** 12/27 : ABP의 zero Divide 문제가 있어서 Max(deltatime, 0.00001f) 노드 추가
  *	추가 위치 : Setting yaw delta for leans 섹션
  * Path : Project_Requiem\Content\Assets\LastBoss\ParagonSevarog\Characters\Heroes\Sevarog\Sevarog_AnimBlueprint.uasset()
  */

/**
 * 
 */
UCLASS()
class PROJECT_REQUIEM_API ALastBossCharacter : public ABaseCharacter, public IBossControlInterface
{
	GENERATED_BODY()

public:
	ALastBossCharacter();

	virtual void BeginPlay() override;
	
public:
	// 델리게이트
	FOnApplyExp OnApplyExp;

	FOnLastBossSpawn OnLastBossSpawn;
	FOnLastBossChangedPhase OnLastBossChangedPhase;
	FOnLastBossEndChangedPhase OnLastBossEndChangedPhase;
	FOnLastBossStop	OnLastBossStop;

	FOnMontageEnded OnMontageEnded;

	UPROPERTY(BlueprintAssignable, Category = "Boss|Interface")
	FOnBossStatUpdated OnBossStatUpdated;
	virtual FOnBossStatUpdated& GetBossStatDelegate() override { return OnBossStatUpdated; }

	UPROPERTY(BlueprintAssignable, Category = "Boss|Interface")
	FOnBossDeathUpdated OnBossDeathUpdated;
	virtual FOnBossDeathUpdated& GetBossDeathDelegate() override { return OnBossDeathUpdated; }

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

	inline const TArray<TObjectPtr<class UAnimMontage>> GetPhaseOnePatterns() const { return PhaseOnePatterns; }

	inline const TArray<TObjectPtr<class UAnimMontage>> GetPhaseTwoPatterns() const { return PhaseTwoPatterns; }


protected:
	// 보스 패턴 배열에 몽타주 추가 함수
	void AddPatternMontage();

private:
	// 보스가 스폰할 때 실행하는 함수
	void LastBossSpawn(UAnimMontage* Montage);
	// 보스가 실행 끝났을 때 실행하는 함수
	void LastBossEndSpawn(UAnimMontage* Montage, bool bInterrupted);
	// 보스의 페이즈 변경 시 실행되는 함수
	void LastBossPhaseChage(UAnimMontage* Montage);
	// 보스가 페이즈 변경 종료 시 실행되는 함수
	void LastBossEndPhaseChage(UAnimMontage* Montage, bool bInterrupted);
	// 보스가 죽었을 때 실행되는 함수
	void LastBossDead(UAnimMontage* Montage);
	// 보스가 죽고 난 후 실행되는 함수
	void LastBossEndDead(UAnimMontage* Montage, bool bInterrupted);

public:
	// 보스 방 입장 시 실행 함수
	virtual void ActivateBossBattle() override;

	// 플레이어 사망 시 실행 함수
	virtual void ResetBossToDefault() override;

private:
	// 보스 초기화 함수
	void ResetLastBoss();

protected:
	// 투사체 스폰 위치
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<class USceneComponent> SpawnProjectileLocation = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh")
	TObjectPtr<class USkeletalMesh> PhaseTwoSkeletalMesh = nullptr;

	// 보스 상태에 따라 실행될 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage")	// 보스가 스폰될 때
	TObjectPtr<class UAnimMontage> SpawnMontage = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage")	// 보스의 페이즈가 변경 될 때
	TObjectPtr<class UAnimMontage> PhaseChangeMontage = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage")	// 보스가 죽었을 때
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
	// 페이즈1 몽타주
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Montage|Patterns")
	TArray<TObjectPtr<class UAnimMontage>> PhaseOnePatterns;

	// 페이즈2 몽타주
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Montage|Patterns")
	TArray<TObjectPtr<class UAnimMontage>> PhaseTwoPatterns;

	// 보스의 드롭 EXP
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "00_Setting|Exp", meta = (ClampMin = "0"))
	float DropExp = 0.0f;

	// 보스 페이즈 상태
	UPROPERTY(VisibleAnywhere, Category = "Information|Phase")
	int32 Phase = 1;

	// 페이즈 변경 확인용
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Information|Phase")
	bool bPhaseChanged = false;

	// 보스 이름
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Information|Name")
	FText BossName;

	// 보스 무적 상태
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Information")
	bool bLastBossInvincible = false;
private:
	FVector SpawnLocation;

	float MinHp = 0.0f;
};
