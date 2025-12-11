// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BossBase/BossBase.h"
#include "Stage1Boss.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_REQUIEM_API AStage1Boss : public ABossBase
{
	GENERATED_BODY()
	
public:
	AStage1Boss();

protected:
	virtual void BeginPlay() override;

};
