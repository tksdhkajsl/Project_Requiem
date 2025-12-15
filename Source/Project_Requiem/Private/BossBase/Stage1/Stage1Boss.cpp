// Fill out your copyright notice in the Description page of Project Settings.


#include "BossBase/Stage1/Stage1Boss.h"

AStage1Boss::AStage1Boss()
{
	MaxHP = 800.0f;
	EXP = 300.0f;

	bUsePhaseSystem = true;
	Phase2StartHPRatio = 0.5f;

	bUseRangedAttack = false;

	Phase2_WalkSpeedMultiplier = 1.0f;
	Phase2_MeleeDamageMultiplier = 1.0f;


}

void AStage1Boss::BeginPlay()
{
	Super::BeginPlay();
}

void AStage1Boss::OnPhaseChanged_Implementation(int32 NewPhase, int32 OldPhase)
{
	Super::OnPhaseChanged_Implementation(NewPhase, OldPhase);

	if (NewPhase == 2)
	{
		bUseRangedAttack = true;
	}

}
