#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PRGameInstance.generated.h"

UCLASS()
class PROJECT_REQUIEM_API UPRGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	bool bHasPlayedIntroVideo = false;

	UPROPERTY()
	bool bIsBoss1Cleared = false;
	UPROPERTY()
	bool bIsBoss2Cleared = false;
	UPROPERTY()
	bool bIsLastBossCleared = false;

	// 현재 플레이어가 도전 중인 문을 기억 (리셋용)
	UPROPERTY()
	class ABossDoor* CurrentChallengeDoor = nullptr;
};
