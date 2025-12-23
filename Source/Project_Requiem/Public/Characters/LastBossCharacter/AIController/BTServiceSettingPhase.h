// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTServiceSettingPhase.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_REQUIEM_API UBTServiceSettingPhase : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTServiceSettingPhase();

protected:
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard|Key")
	FBlackboardKeySelector PhaseKey;
	UPROPERTY(EditAnywhere, Category = "Blackboard|Key")
	FBlackboardKeySelector MontageNumberKey;

private:
	TWeakObjectPtr<class ALastBossCharacter> LastBoss = nullptr;
	TWeakObjectPtr<class UBlackboardComponent> BlackBoardComp = nullptr;
};
