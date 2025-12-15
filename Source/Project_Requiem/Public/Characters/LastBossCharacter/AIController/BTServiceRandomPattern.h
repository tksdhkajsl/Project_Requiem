// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTServiceRandomPattern.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_REQUIEM_API UBTServiceRandomPattern : public UBTService
{
	GENERATED_BODY()
	
protected:
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	TWeakObjectPtr<class ALastBossCharacter> LastBoss = nullptr;
};
