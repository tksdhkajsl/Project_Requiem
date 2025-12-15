// Fill out your copyright notice in the Description page of Project Settings.


#include "BossBase/Stage2/Stage2Boss.h"

AStage2Boss::AStage2Boss()
{
	MaxHP = 1500.0f;
	EXP = 800.0f;

	bUsePhaseSystem = true;
	Phase2StartHPRatio = 0.5f;
	Phase2_WalkSpeedMultiplier = 1.3f;
	Phase2_MeleeDamageMultiplier = 1.7f;
}

void AStage2Boss::BeginPlay()
{
	Super::BeginPlay();
}

void AStage2Boss::OnPhaseChanged_Implementation(int32 NewPhase, int32 OldPhase)
{
	Super::OnPhaseChanged_Implementation(NewPhase, OldPhase);

	if (NewPhase == 2)
	{

	}
}
