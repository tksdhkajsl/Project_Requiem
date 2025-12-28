// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTServiceChasePlayer.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_REQUIEM_API UBTServiceChasePlayer : public UBTService
{
	GENERATED_BODY()
	
protected:
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	// 플레이어 위치값을 저장할 블랙보드 키
	UPROPERTY(EditAnywhere, Category = "Blackboard|Key")
	FBlackboardKeySelector TargetPlayerLocationKey;

private:
	UPROPERTY()
	TWeakObjectPtr<class UBlackboardComponent> BlackBoardComp = nullptr;
};
