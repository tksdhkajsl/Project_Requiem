#include "Items/Data/ItemActor.h"
#include "Items/Data/ItemBase.h"

AItemActor::AItemActor()
{
}

void AItemActor::InitializeFromItem(UItemBase* Item)
{
    if (!Item) return;

    ItemRowHandle = Item->ItemRowHandle;
    Quantity = Item->Quantity;

    UpdateMeshFromItemData();
}

void AItemActor::InitializeFromRowHandle(const FDataTableRowHandle& InRowHandle, int32 InQuantity)
{
    ItemRowHandle = InRowHandle;
    Quantity = InQuantity;

    UpdateMeshFromItemData();
}

void AItemActor::UpdateMeshFromItemData()
{
    if (!ItemRowHandle.DataTable || ItemRowHandle.RowName.IsNone())
        return;

    const FItemData* Data =
        ItemRowHandle.DataTable->FindRow<FItemData>(
            ItemRowHandle.RowName,
            TEXT("ItemActor Mesh Update")
        );

    if (Data && Data->Mesh)
    {
        MeshComp->SetStaticMesh(Data->Mesh);
    }
}
