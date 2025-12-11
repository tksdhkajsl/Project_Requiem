// Fill out your copyright notice in the Description page of Project Settings.


#include "BossBase/Stage1/Stage1Boss.h"

AStage1Boss::AStage1Boss()
{
	MaxHP = 800.0f;
	EXP = 300.0f;
	bUsePhaseSystem = false;
}

void AStage1Boss::BeginPlay()
{
	Super::BeginPlay();
}
