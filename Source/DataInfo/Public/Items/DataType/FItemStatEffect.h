#pragma once

#include "CoreMinimal.h"
#include "Stats/Data/EFullStats.h"
#include "FItemStatEffect.generated.h"

USTRUCT(BlueprintType)
struct FItemStatEffect
{
    GENERATED_BODY()

public:
    /** 변경할 스탯 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    EFullStats StatType;

    /** 변화량 (+ / - 모두 가능) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    float DeltaValue = 0.f;
};