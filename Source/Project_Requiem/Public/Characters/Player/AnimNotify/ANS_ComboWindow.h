// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_ComboWindow.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_REQUIEM_API UANS_ComboWindow : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyTick(
		USkeletalMeshComponent* MeshComp, 
		UAnimSequenceBase* Animation, 
		float FrameDeltaTime, 
		const FAnimNotifyEventReference& EventReference) override;
	
};
