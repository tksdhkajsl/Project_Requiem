// Stage2Boss.cpp
#include "BossBase/Stage2/Stage2Boss.h"
#include "Internationalization/Text.h"

AStage2Boss::AStage2Boss()
{
	MaxHP = 1500.0f;
	EXP = 800.0f;

	// 2페이즈 보스
	bUsePhaseSystem = true;
	Phase2StartHPRatio = 0.5f;

	// 2페이즈 스탯 강화
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
		// 여기에서 2페이즈 전용 패턴 ON, 이펙트, 원거리 ON, 광폭화 등 추가하면 됨
	}
}
