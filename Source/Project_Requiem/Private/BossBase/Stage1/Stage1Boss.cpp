// Stage1Boss.cpp
#include "BossBase/Stage1/Stage1Boss.h"
#include "ComponentSystems/Public/Stats/StatComponent.h"
#include "Internationalization/Text.h"

AStage1Boss::AStage1Boss()
{
	if (GetStatComponent())
	{
		GetStatComponent()->CurrHP = 800.0f;
	}

	EXP = 300.0f;

	bUsePhaseSystem = true;
	Phase2StartHPRatio = 0.5f;

	bUseRangedAttack = false;

	Phase2_WalkSpeedMultiplier = 1.0f;
	Phase2_MeleeDamageMultiplier = 1.0f;

	// 보스 이름
	BossName = FText::FromString(TEXT("사체포식자"));
}

void AStage1Boss::BeginPlay()
{
	Super::BeginPlay();
}

void AStage1Boss::OnPhaseChanged_Implementation(int32 NewPhase, int32 OldPhase)
{
	Super::OnPhaseChanged_Implementation(NewPhase, OldPhase);

	// 디버그 로그: 페이즈 전환 확인
	UE_LOG(LogTemp, Warning, TEXT("Stage1Boss Phase Changed: %d -> %d"), OldPhase, NewPhase);

	if (NewPhase == 2)
	{
		// 2페이즈부터 원거리 공격 시작
		bUseRangedAttack = true;
	}
}
