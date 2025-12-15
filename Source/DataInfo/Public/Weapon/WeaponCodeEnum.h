// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

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
