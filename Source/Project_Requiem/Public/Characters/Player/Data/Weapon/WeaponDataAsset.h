// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Weapon/WeaponCodeEnum.h"
#include "WeaponDataAsset.generated.h"


class AWeaponActor;
/**
 * 
 */
UCLASS()
class PROJECT_REQUIEM_API UWeaponDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	// 무기 종류(식별용)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Type")
	EWeaponCode WeaponType = EWeaponCode::OneHandedSword;

	// 손에 들리는 무기 클래스
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Actor Classes")
	TSubclassOf<AWeaponActor> EquippedWeaponClass;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Weapon")
	bool IsValid() const
	{
		return EquippedWeaponClass != nullptr;
	}

};
