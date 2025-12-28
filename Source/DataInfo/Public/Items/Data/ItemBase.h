#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Engine/DataTable.h"
#include "Stats/Data/EFullStats.h"
#include "Items/DataTable/FItemData.h"
#include "ItemBase.generated.h"

UCLASS(BlueprintType, Blueprintable)
class DATAINFO_API UItemBase : public UObject
{
    GENERATED_BODY()

public:
    UItemBase();

    /** 어떤 아이템인가 (정의 참조) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    FDataTableRowHandle ItemRowHandle;

    /** 인벤토리 수량 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    int32 Quantity = 1;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory") 
    FIntPoint ItemSize;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item | Stats")
    TMap<EFullStats, float> StatModifiers;


public:
    /** DT에서 아이템 정의 가져오기 */
    const FItemData* GetItemData() const;

    /** 유효성 체크 */
    bool IsValidItem() const;

    /** 스택 가능 여부 */
    bool IsStackable() const;

    /** 스택 초과 여부 */
    bool IsFull() const;

public:
    UFUNCTION(BlueprintCallable, Category = "Item")
    virtual void Use(AActor* User);

    UFUNCTION(BlueprintCallable, Category = "Item")
    virtual void Place(AActor* User);

protected:
    /** 캐시 (런타임 조회 최적화용) */
    mutable const FItemData* CachedItemData = nullptr;

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
