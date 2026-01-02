// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Weapon/WeaponCodeEnum.h"
#include "WeaponMasteryComponent.generated.h"

#pragma region 플레이어 데이터 저장용
USTRUCT()
struct FWeaponMasterySaveData
{
	GENERATED_BODY()

	UPROPERTY()
	EWeaponRank OneHandedRank = EWeaponRank::F;

	UPROPERTY()
	EWeaponRank TwoHandedRank = EWeaponRank::F;

	UPROPERTY()
	EWeaponRank DualBladeRank = EWeaponRank::F;
};
#pragma endregion

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_REQUIEM_API UWeaponMasteryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeaponMasteryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// 1. 적 처치 시 호출: 해당 무기의 킬 카운트 증가
	UFUNCTION(BlueprintCallable)
	void AddKillCount(EWeaponCode WeaponType);

	// 2. 현재 무기의 숙련도 정보 가져오기 (공격할 때 씀)
	FWeaponMasteryData GetMasteryData(EWeaponCode WeaponType) const;

	// 3. [추가] 12/31, 현재 무기의 '랭크(Enum)'만 가져오기 (UI 표시용)
	UFUNCTION(BlueprintCallable, Category = "Mastery")
	EWeaponRank GetWeaponRank(EWeaponCode WeaponType) const;

private:
	// 내부 함수: 랭크업 체크 및 스탯 업데이트
	void CheckRankUp(FWeaponMasteryData& MasteryData, EWeaponCode WeaponType);

	// 내부 함수: 랭크에 따른 스탯 테이블 (기획표 내용)
	void UpdateStatsByRank(FWeaponMasteryData& MasteryData);

protected:
	// 변수: 각 무기별 숙련도 데이터 저장
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mastery")
	FWeaponMasteryData OneHandedMastery; // 한손

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mastery")
	FWeaponMasteryData TwoHandedMastery; // 두손

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mastery")
	FWeaponMasteryData DualBladeMastery; // 쌍검
		

#pragma region 플레이어 데이터 저장용
public:
	FWeaponMasterySaveData SaveData() const { return { OneHandedMastery.CurrentRank,	TwoHandedMastery.CurrentRank,	DualBladeMastery.CurrentRank }; };
	void LoadData(const FWeaponMasterySaveData& Data)
	{
		OneHandedMastery.CurrentRank = Data.OneHandedRank;
		TwoHandedMastery.CurrentRank = Data.TwoHandedRank;
		DualBladeMastery.CurrentRank = Data.DualBladeRank;

		UpdateStatsByRank(OneHandedMastery);
		UpdateStatsByRank(TwoHandedMastery);
		UpdateStatsByRank(DualBladeMastery);
	}

#pragma endregion
};
