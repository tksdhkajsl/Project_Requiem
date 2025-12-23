// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/LastBossCharacter/AIController/LastBossAIController.h"

void ALastBossAIController::BeginPlay()
{
	if (BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);
	}
}
