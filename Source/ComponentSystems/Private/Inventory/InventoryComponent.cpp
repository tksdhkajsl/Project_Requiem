#include "Inventory/InventoryComponent.h"

UInventoryComponent::UInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::BeginPlay()
{
    Super::BeginPlay();

    // Grid 초기화
    OccupiedGrid.SetNum(GridWidth * GridHeight);
    for (UItemBase*& Cell : OccupiedGrid)
    {
        Cell = nullptr;
    }
}

/* =========================
 * 내부 유틸
 * ========================= */

int32 UInventoryComponent::ToIndex(const FIntPoint& Pos) const
{
    return Pos.Y * GridWidth + Pos.X;
}

bool UInventoryComponent::IsInBounds(const FIntPoint& Pos) const
{
    return Pos.X >= 0 && Pos.Y >= 0 &&
        Pos.X < GridWidth && Pos.Y < GridHeight;
}

bool UInventoryComponent::CanPlaceItem(UItemBase* Item, const FIntPoint& StartPos) const
{
    if (!Item) return false;

    const FIntPoint Size = Item->ItemSize;

    for (int32 y = 0; y < Size.Y; ++y)
    {
        for (int32 x = 0; x < Size.X; ++x)
        {
            FIntPoint CheckPos = StartPos + FIntPoint(x, y);

            if (!IsInBounds(CheckPos))
            {
                return false;
            }

            int32 Index = ToIndex(CheckPos);
            if (OccupiedGrid.IsValidIndex(Index) && OccupiedGrid[Index] != nullptr)
            {
                return false;
            }
        }
    }

    return true;
}

void UInventoryComponent::SetItemOccupancy(UItemBase* Item, const FIntPoint& StartPos, bool bOccupy)
{
    if (!Item) return;

    const FIntPoint Size = Item->ItemSize;

    for (int32 y = 0; y < Size.Y; ++y)
    {
        for (int32 x = 0; x < Size.X; ++x)
        {
            FIntPoint Pos = StartPos + FIntPoint(x, y);
            int32 Index = ToIndex(Pos);

            if (OccupiedGrid.IsValidIndex(Index))
            {
                OccupiedGrid[Index] = bOccupy ? Item : nullptr;
            }
        }
    }
}

/* =========================
 * Public API
 * ========================= */

bool UInventoryComponent::AddItem(TSubclassOf<UItemBase> ItemClass, const FIntPoint& StartPos)
{
    if (!ItemClass) return false;

    UItemBase* NewItem = NewObject<UItemBase>(this, ItemClass);

    if (!NewItem || !CanPlaceItem(NewItem, StartPos)) return false;

    FInventoryGridItem NewGridItem;
    NewGridItem.Item = NewItem;
    NewGridItem.GridPos = StartPos;

    Items.Add(NewGridItem);

    // 4. 그리드 칸 차지 설정
    SetItemOccupancy(NewItem, StartPos, true);

    OnItemAdded.Broadcast(NewItem, StartPos);

    return true;
}

bool UInventoryComponent::RemoveItem(UItemBase* Item)
{
    if (!Item) return false;

    for (int32 i = 0; i < Items.Num(); ++i)
    {
        if (Items[i].Item == Item)
        {
            SetItemOccupancy(Item, Items[i].GridPos, false);
            Items.RemoveAt(i);
            return true;
        }
    }

    return false;
}

bool UInventoryComponent::MoveItem(UItemBase* Item, const FIntPoint& NewPos)
{
    if (!Item) return false;

    for (FInventoryGridItem& GridItem : Items)
    {
        if (GridItem.Item == Item)
        {
            SetItemOccupancy(Item, GridItem.GridPos, false);

            if (!CanPlaceItem(Item, NewPos))
            {
                SetItemOccupancy(Item, GridItem.GridPos, true);
                return false;
            }

            GridItem.GridPos = NewPos;
            SetItemOccupancy(Item, NewPos, true);

            if (OnInventoryUpdated.IsBound())
            {
                OnInventoryUpdated.Broadcast();
            }
            return true;
        }
    }
    return false;
}

UItemBase* UInventoryComponent::GetItemAt(const FIntPoint& GridPos) const
{
    if (!IsInBounds(GridPos)) return nullptr;

    int32 Index = ToIndex(GridPos);
    return OccupiedGrid.IsValidIndex(Index) ? OccupiedGrid[Index] : nullptr;
}

const TArray<FInventoryGridItem>& UInventoryComponent::GetAllItems() const
{
    return Items;
}
