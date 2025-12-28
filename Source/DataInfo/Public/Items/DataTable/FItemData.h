#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Items/DataType/EItemType.h"
#include "FItemData.generated.h"

USTRUCT(BlueprintType)
struct FItemData : public FTableRowBase
{
    GENERATED_BODY()

public:
    /* =========================
     * Basic Info
     * ========================= */

     // 아이템 표시 이름 (UI용)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic")
    FText DisplayName;

    // 아이템 설명 (UI용)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic")
    FText Description;

    // 아이템 분류
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic")
    EItemType ItemType = EItemType::None;

    // 인벤토리 아이콘
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic")
    UTexture2D* Icon = nullptr;

    // 월드 배치 / 드롭용 메쉬
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic")
    UStaticMesh* Mesh = nullptr;

    /* =========================
     * Stats
     * ========================= */

     // 최대 스택 수 (1이면 스택 불가)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
    int32 MaxStackCount = 1;

    // 상점 구매 가격
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
    int32 BuyPrice = 0;

    // 상점 판매 가격 (0이면 판매 불가)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
    int32 SellPrice = 0;

     // 사용 가능 여부 (포션, 음식 등)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Function")
    bool bIsUsable = false;

    // 월드 배치 가능 여부
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Function")
    bool bIsPlaceable = false;

    // 장착 가능 여부 (무기, 방어구)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Function")
    bool bIsEquipable = false;

public:
    FItemData() {}
};