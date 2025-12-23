#pragma once

#include "CoreMinimal.h"
#include "EFullStats.generated.h"

/**
 * @brief 실제 캐릭터 스탯창에 나오는 "모든 스탯" 열거형
 */
UENUM(BlueprintType)
enum class EFullStats : uint8
{
    None UMETA(Hidden),

    // -----------------------------
    // 힘 올릴시
    // -----------------------------
    Health         UMETA(DisplayName = "체력", Category = "Regen"),
    PhysicalAttack UMETA(DisplayName = "물공", Category = "NoRegen"),

    // -----------------------------
    // 민첩 올릴시
    // -----------------------------
    Stamina     UMETA(DisplayName = "기력", Category = "Regen"),
    AttackSpeed UMETA(DisplayName = "공속", Category = "NoRegen"),

    // -----------------------------
    // 기타
    // -----------------------------
    MagicAttack     UMETA(DisplayName = "마공", Category = "NoRegen"),
    PhysicalDefense UMETA(DisplayName = "물방", Category = "NoRegen"),
    MagicDefense    UMETA(DisplayName = "마방", Category = "NoRegen"),

    MAX UMETA(Hidden)
};
