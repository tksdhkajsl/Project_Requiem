#pragma once

#include "CoreMinimal.h"
#include "ELevelUpStats.generated.h"

/**
 * @brief 플레이어가 직접 포인트를 투자할 수 있는 "레벨업 스탯" 열거형
 * 이 Enum은 캐릭터가 레벨업 시 배분할 수 있는 스탯 종류를 정의.
 */
UENUM(BlueprintType, meta = (ScriptName = "LevelUpStatTypes"))
enum class ELevelUpStats : uint8
{

    // 전투 관련
    Strength    UMETA(DisplayName = "힘"),      /** 체력/물리 공격력 */
    Dexterity   UMETA(DisplayName = "민첩"),    /** 기력/공격속도 */
};