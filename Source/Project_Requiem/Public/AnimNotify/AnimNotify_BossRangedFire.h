// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_BossRangedFire.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_REQUIEM_API UAnimNotify_BossRangedFire : public UAnimNotify
{
	GENERATED_BODY()
	

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	FName SocketName = NAME_None;



};
