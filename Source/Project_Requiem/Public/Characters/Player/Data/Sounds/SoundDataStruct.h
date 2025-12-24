// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Sound/SoundBase.h"
#include "SoundDataStruct.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FSoundData : public FTableRowBase
{
	GENERATED_BODY()

public:
	// 사운드 에셋
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	TObjectPtr<USoundBase> SoundAsset;

	// (선택 사항) 볼륨 조절이 필요하다면 추가
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	float VolumeMultiplier = 1.0f;

	// (선택 사항) 피치 조절이 필요하다면 추가
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	float PitchMultiplier = 1.0f;
};