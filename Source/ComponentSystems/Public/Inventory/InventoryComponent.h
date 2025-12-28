#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Items/Data/ItemBase.h"
#include "Inventory/FInventoryGridItem.h"

#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemAdded, UItemBase*, Item, FIntPoint, StartPos);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COMPONENTSYSTEMS_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable, Category = "Inventory")
    FOnItemAdded OnItemAdded;
    UPROPERTY(BlueprintAssignable, Category = "Inventory")
    FOnInventoryUpdated OnInventoryUpdated;

public:
    UInventoryComponent();

protected:
    virtual void BeginPlay() override;

protected:
    /** 인벤토리 가로 / 세로 크기 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
    int32 GridWidth = 10;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
    int32 GridHeight = 6;

    /** 실제 아이템 배치 정보 */
    UPROPERTY()
    TArray<FInventoryGridItem> Items;

    /** 빠른 점유 체크용 (Width * Height) */
    UPROPERTY()
    TArray<UItemBase*> OccupiedGrid;

protected:
    /** 내부 유틸: 좌표 → 인덱스 */
    int32 ToIndex(const FIntPoint& Pos) const;

    /** 내부 유틸: 범위 체크 */
    bool IsInBounds(const FIntPoint& Pos) const;

    /** 내부 유틸: 해당 영역 비어있는지 */
    bool CanPlaceItem(UItemBase* Item, const FIntPoint& StartPos) const;

    /** 내부 유틸: Grid 점유 / 해제 */
    void SetItemOccupancy(UItemBase* Item, const FIntPoint& StartPos, bool bOccupy);

public:
    /** 아이템 추가 */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool AddItem(TSubclassOf<UItemBase> ItemClass, const FIntPoint& StartPos);

    /** 아이템 제거 */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool RemoveItem(UItemBase* Item);

    /** 아이템 이동 */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool MoveItem(UItemBase* Item, const FIntPoint& NewPos);

    /** 해당 위치에 아이템이 있는지 */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    UItemBase* GetItemAt(const FIntPoint& GridPos) const;

    /** 전체 아이템 목록 (UI용) */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    const TArray<FInventoryGridItem>& GetAllItems() const;
};