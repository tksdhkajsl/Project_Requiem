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
	virtual void OnPossess(APawn* InPawn) override;

private:
	UFUNCTION()
	void StartBehaviorTree();

	UFUNCTION()
	void StopBehaviorTree();

private:
	TWeakObjectPtr<class ALastBossCharacter> LastBoss = nullptr;

	UPROPERTY(EditAnywhere, Category = "BehaviorTree")
	TObjectPtr<UBehaviorTree> BehaviorTree = nullptr;
};
