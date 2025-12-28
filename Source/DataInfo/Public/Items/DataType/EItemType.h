#pragma once

#include "CoreMinimal.h"
#include "EItemType.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
    None        UMETA(DisplayName = "None"),     // 빈 슬롯

    Weapon      UMETA(DisplayName = "Weapon"),   // 장착 아이템
    Food        UMETA(DisplayName = "Food"),     // 즉시 사용
    Potion      UMETA(DisplayName = "Potion"),   // 즉시 사용 (버프/회복)
};