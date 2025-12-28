// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNodeRunMontage.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_REQUIEM_API UBTTaskNodeRunMontage : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	// BT 에셋 로드시 호출
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

	// 테스크 실행 시 호출
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	void MontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void NextPhase();

private:
	TWeakObjectPtr<class ALastBossCharacter> LastBoss = nullptr;
	TWeakObjectPtr<class UBlackboardComponent> BlackBoardComp = nullptr;

	// ExecuteTask의 OwnerComp를 저장할 캐시
	TWeakObjectPtr<UBehaviorTreeComponent> CachedOwnerComp = nullptr;
};
