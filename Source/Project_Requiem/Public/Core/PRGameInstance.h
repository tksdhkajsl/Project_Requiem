#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include <Characters/Player/Weapons/WeaponManagerComponent.h>
#include <Characters/Player/Weapons/WeaponMasteryComponent.h>
#include <Stats/StatComponent.h>

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


#pragma region 플레이어 저장용 데이터(플레이어 스테이트 만들기 싫다)
public:

	UPROPERTY()
	UWeaponMasteryComponent* SavedWeaponMastery = nullptr;

	UPROPERTY()
	UStatComponent* SavedStatComponent = nullptr;
#pragma endregion


};
