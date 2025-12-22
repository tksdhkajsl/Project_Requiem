// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/LastBossCharacter/AIController/LastBossAIController.h"
#include "Characters/LastBossCharacter/LastBossCharacter.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BrainComponent.h"

void ALastBossAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// 보스의 스폰 몽타주 종료 이후 비헤이비어 트리 활성화
	if (!LastBoss.IsValid())
		LastBoss = Cast<ALastBossCharacter>(InPawn);

	if (LastBoss.IsValid())
	{
		// 보스 스폰시 
		LastBoss->OnLastBossSpawn.RemoveDynamic(this, &ALastBossAIController::StartBehaviorTree);
		LastBoss->OnLastBossSpawn.AddDynamic(this, &ALastBossAIController::StartBehaviorTree);

		// 페이즈 전환 시작시 
		LastBoss->OnLastBossChangedPhase.RemoveDynamic(this, &ALastBossAIController::StopBehaviorTree);
		LastBoss->OnLastBossChangedPhase.AddDynamic(this, &ALastBossAIController::StopBehaviorTree);

		// 페이즈 전환 종료시
		LastBoss->OnLastBossEndChangedPhase.RemoveDynamic(this, &ALastBossAIController::StartBehaviorTree);
		LastBoss->OnLastBossEndChangedPhase.AddDynamic(this, &ALastBossAIController::StartBehaviorTree);

		// 보스 사망시
		LastBoss->OnLastBossDead.RemoveDynamic(this, &ALastBossAIController::StopBehaviorTree);
		LastBoss->OnLastBossDead.AddDynamic(this, &ALastBossAIController::StopBehaviorTree);
	}
}

void ALastBossAIController::StartBehaviorTree()
{
	if (BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);
	}
}

void ALastBossAIController::StopBehaviorTree()
{
	if (BehaviorTree)
	{
		if (UBehaviorTreeComponent* BTComp = Cast<UBehaviorTreeComponent>(GetBrainComponent()))
		{
			BTComp->StopTree(EBTStopMode::Forced);
		}
	}
}
