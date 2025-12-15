// Stage1Boss.cpp
#include "BossBase/Stage1/Stage1Boss.h"
#include "Internationalization/Text.h"

AStage1Boss::AStage1Boss()
{
	MaxHP = 800.0f;
	EXP = 300.0f;

	// 페이즈 사용(현재 코드는 1스테도 2페이즈 존재)
	bUsePhaseSystem = true;
	Phase2StartHPRatio = 0.5f;

	// 시작은 원거리 OFF (2페이즈에서 켜도록)
	bUseRangedAttack = false;

	// 1스테는 페이즈2에서도 스탯 변화 없게(=1.0)
	Phase2_WalkSpeedMultiplier = 1.0f;
	Phase2_MeleeDamageMultiplier = 1.0f;

	// 보스 이름(UI 표기용)
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
		// 2페이즈부터 원거리 사용 시작
		bUseRangedAttack = true;
	}
}
