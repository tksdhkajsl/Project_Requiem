// Stage2Boss.h
#pragma once

#include "CoreMinimal.h"
#include "BossBase/BossBase.h"
#include "Stage2Boss.generated.h"

UCLASS()
class PROJECT_REQUIEM_API AStage2Boss : public ABossBase
{
	GENERATED_BODY()

public:
	AStage2Boss();

protected:
	virtual void BeginPlay() override;

	virtual void OnPhaseChanged_Implementation(int32 NewPhase, int32 OldPhase) override;
};
