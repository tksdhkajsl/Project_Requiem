// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "LastBossAIController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_REQUIEM_API ALastBossAIController : public AAIController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "BehaviorTree")
	TObjectPtr<UBehaviorTree> BehaviorTree = nullptr;
};
