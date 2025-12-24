#include "Items/Data/ItemBase.h"

UItemBase::UItemBase()
{
    Quantity = 1;
}

const FItemData* UItemBase::GetItemData() const
{
    if (CachedItemData)
        return CachedItemData;

    if (!ItemRowHandle.DataTable || ItemRowHandle.RowName.IsNone())
        return nullptr;

    CachedItemData =
        ItemRowHandle.DataTable->FindRow<FItemData>(
            ItemRowHandle.RowName,
            TEXT("GetItemData")
        );

    return CachedItemData;
}

bool UItemBase::IsValidItem() const
{
    return GetItemData() != nullptr;
}

bool UItemBase::IsStackable() const
{
    const FItemData* Data = GetItemData();
    return Data && Data->MaxStackCount > 1;
}

bool UItemBase::IsFull() const
{
    const FItemData* Data = GetItemData();
    return Data && Quantity >= Data->MaxStackCount;
}

void UItemBase::Use(AActor* User)
{
    const FItemData* Data = GetItemData();
    if (!Data || !User) return;

    if (Data->bIsPlaceable)
    {
        Place(User);
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("Use Item: %s"), *Data->DisplayName.ToString());

    //UStatComponent* StatComp = User->FindComponentByClass<UStatComponent>();
    //if (StatComp) {
    //    for (const TPair<EFullStats, float>& StatPair : StatModifiers)
    //    {
    //        StatComp->ChangeStatCurrent(StatPair.Key, StatPair.Value);
    //    }
    //}
}

void UItemBase::Place(AActor* User)
{
    const FItemData* Data = GetItemData();
    if (!Data) return;

    UE_LOG(LogTemp, Log, TEXT("Place Item: %s"), *Data->DisplayName.ToString());
}

#if WITH_EDITOR
void UItemBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    const FName PropertyName =
        PropertyChangedEvent.Property
        ? PropertyChangedEvent.Property->GetFName()
        : NAME_None;

    if (PropertyName == GET_MEMBER_NAME_CHECKED(UItemBase, ItemRowHandle) ||
        PropertyName == GET_MEMBER_NAME_CHECKED(FDataTableRowHandle, RowName))
    {
        CachedItemData = nullptr; // 캐시 무효화
    }
}
#endif