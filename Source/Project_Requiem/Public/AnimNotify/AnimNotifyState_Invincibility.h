// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_Invincibility.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_REQUIEM_API UAnimNotifyState_Invincibility : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	// 애니메이션의 해당 구간이 시작될 때 호출됩니다.
	virtual void NotifyBegin(
		USkeletalMeshComponent* MeshComp, 
		UAnimSequenceBase* Animation,
		float TotalDuration) override;

	// 애니메이션의 해당 구간이 끝날 때 호출됩니다.
	virtual void NotifyEnd(
		USkeletalMeshComponent* MeshComp, 
		UAnimSequenceBase* Animation) override;

};
