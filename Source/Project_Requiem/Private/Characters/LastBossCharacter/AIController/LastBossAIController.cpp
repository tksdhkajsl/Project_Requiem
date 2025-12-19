// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/LastBossCharacter/AIController/LastBossAIController.h"
#include "Characters/LastBossCharacter/LastBossCharacter.h"

void ALastBossAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// 보스의 스폰 몽타주 종료 이후 비헤이비어 트리 활성화
	if (!LastBoss.IsValid())
		LastBoss = Cast<ALastBossCharacter>(InPawn);

	if (LastBoss.IsValid())
	{
		LastBoss->OnLastBossSpawn.RemoveDynamic(this, &ALastBossAIController::StartBehaviorTree);
		LastBoss->OnLastBossSpawn.AddDynamic(this, &ALastBossAIController::StartBehaviorTree);
	}
}

void ALastBossAIController::StartBehaviorTree()
{
	if (BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);
	}
}
