// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Weapons/WeaponActor.h"
#include "Common/CommonEnum.h"
#include "WeaponDataTable.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FWeaponTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	// 1. 무기 종류 (이걸로 검색할 겁니다)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWeaponCode WeaponCode;

	// 2. 소환할 무기 블루프린트 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AWeaponActor> WeaponClass;

	// (선택사항) 아이콘, 공격력, 이름 등을 여기에 추가하면 엑셀처럼 관리 가능!
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// float AttackPower;
};