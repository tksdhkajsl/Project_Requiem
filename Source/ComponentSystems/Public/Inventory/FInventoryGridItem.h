#pragma once
#include "CoreMinimal.h"
#include "Items/Data/ItemBase.h"
#include "FInventoryGridItem.generated.h"

USTRUCT(BlueprintType)
struct FInventoryGridItem
{
    GENERATED_BODY()

public:
    /** 실제 아이템 */
    UPROPERTY()
    UItemBase* Item = nullptr;

    /** 인벤토리 그리드 기준 좌상단 위치 */
    UPROPERTY()
    FIntPoint GridPos = {0,0};
};