// Stage2Boss.cpp
#include "BossBase/Stage2/Stage2Boss.h"
#include "ComponentSystems/Public/Stats/StatComponent.h"
#include "Internationalization/Text.h"

AStage2Boss::AStage2Boss()
{
	GetStatComponent()->CurrHP = 1500.0f;
	EXP = 800.0f;

	bUsePhaseSystem = true;
	Phase2StartHPRatio = 0.5f;

	Phase2_WalkSpeedMultiplier = 1.3f;
	Phase2_MeleeDamageMultiplier = 1.7f;

	BossName = FText::FromString(TEXT("한기의 집행자"));
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
		// 2페이즈 전용 패턴 추가
	}
}
