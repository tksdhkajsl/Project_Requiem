// Stage1Boss.h
#pragma once

#include "CoreMinimal.h"
#include "BossBase/BossBase.h"
#include "Stage1Boss.generated.h"

UCLASS()
class PROJECT_REQUIEM_API AStage1Boss : public ABossBase
{
	GENERATED_BODY()

public:
	AStage1Boss(); // 스탯/이름/페이즈 옵션을 여기서 지정

protected:
	virtual void BeginPlay() override;

	// 페이즈 변경 시 추가 행동(예: 2페이즈에서 원거리 On)
	virtual void OnPhaseChanged_Implementation(int32 NewPhase, int32 OldPhase) override;
};
