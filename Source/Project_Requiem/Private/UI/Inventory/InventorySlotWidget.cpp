#include "UI/Inventory/InventorySlotWidget.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"

void UInventorySlotWidget::NativeConstruct()
{
    Super::NativeConstruct();

    UpdateVisual();
}

void UInventorySlotWidget::UpdateVisual()
{
    if (!SlotImage) return;

    if (bOccupied) SlotImage->SetBrushFromTexture(OccupiedTexture);
    else SlotImage->SetBrushFromTexture(UnoccupiedTexture);
}

void UInventorySlotWidget::SetOccupied(bool bNewOccupied)
{
    bOccupied = bNewOccupied;
    UpdateVisual();
}