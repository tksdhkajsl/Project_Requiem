#pragma once

#include "CoreMinimal.h"
#include "EFullStats.generated.h"

/**
 * @brief 실제 캐릭터 스탯창에 나오는 "모든 스탯" 열거형
 */
UENUM(BlueprintType)
enum class EFullStats : uint8
{
    None,

#pragma region Tick Regen 가능한것들(타이머 틱)
    // -----------------------------
    // 주요 자원(Resource)
    // -----------------------------
    Health,
    Stamina,
#pragma endregion

#pragma region 실시간으로 변하는게 아닌 단일 로만 계산되는 애들
    // -----------------------------
    // 공격 관련
    // -----------------------------
    PhysicalAttack,      // 물리 공격력
    MagicAttack,         // 마법 공격력
    // -----------------------------
    // 방어 관련
    // -----------------------------
    PhysicalDefense,     // 물리 방어력
    MagicDefense,        // 마법 방어력

#pragma endregion

    MAX UMETA(Hidden)
};
