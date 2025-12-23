// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponCodeEnum.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EWeaponCode : uint8
{
	OneHandedSword		UMETA(DisplayName = "한손검"),
	TwoHandedSword		UMETA(DisplayName = "양손검"),
	DualBlade			UMETA(DisplayName = "쌍검")
};

// 1. 숙련도 랭크 (F ~ S)
UENUM(BlueprintType)
enum class EWeaponRank : uint8
{
	F UMETA(DisplayName = "F"),
	E UMETA(DisplayName = "E"),
	D UMETA(DisplayName = "D"),
	C UMETA(DisplayName = "C"),
	B UMETA(DisplayName = "B"),
	A UMETA(DisplayName = "A"),
	S UMETA(DisplayName = "S")
};

// 2. 무기 숙련도 구조체 (요청하신 내용: 크확, 크뎀, 몹갯수)
USTRUCT(BlueprintType)
struct FWeaponMasteryData
{
	GENERATED_BODY()

public:
	// 현재 랭크
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EWeaponRank CurrentRank = EWeaponRank::F;

	// 현재 잡은 몹 갯수
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 CurrentKillCount = 0;

	// 다음 랭크업을 위해 잡아야 하는 몹 갯수 (Max)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxKillCount = 3; // F -> E 가는데 3마리라고 가정

	// 현재 랭크의 크리티컬 확률 배율 (예: 0.0, 0.1 ...)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CritRate = 0.0f;

	// 현재 랭크의 크리티컬 데미지 배율 (예: 1.0, 1.2 ...)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CritDamage = 1.0f;
};

USTRUCT(BlueprintType)
struct FWeaponComboData
{
	GENERATED_BODY()

public:
	// 이 무기용 공격 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UAnimMontage> Montage;

	// 이 무기의 최대 콤보 수 (예: 3, 2, 4)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxComboCount = 3;
};