#include "UI/Inventory/InventoryWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Items/Data/ItemBase.h"
#include "Inventory/InventoryComponent.h"

void UInventoryWidget::NativeConstruct()
{
    Super::NativeConstruct();

    CreateSlots();
}

void UInventoryWidget::CreateSlots()
{
    if (!CanvasPanel || !SlotClass) return;

    Slots.Empty();
    CanvasPanel->ClearChildren();

    for (int32 Y = 0; Y < GridHeight; ++Y)
    {
        for (int32 X = 0; X < GridWidth; ++X)
        {
            UInventorySlotWidget* NewSlot = CreateWidget<UInventorySlotWidget>(this, SlotClass);
            if (!NewSlot) continue;

            UCanvasPanelSlot* CanvasSlot = CanvasPanel->AddChildToCanvas(NewSlot);
            CanvasSlot->SetSize(SlotSize);
            CanvasSlot->SetPosition(FVector2D(X * SlotSize.X, Y * SlotSize.Y));

            Slots.Add(NewSlot);
        }
    }
}

void UInventoryWidget::InitInventory(UInventoryComponent* InComp)
{
    if (!InComp) return;

    CachedInvenComp = InComp;

    CachedInvenComp->OnItemAdded.AddDynamic(this, &UInventoryWidget::AddItem);
    CachedInvenComp->OnInventoryUpdated.AddDynamic(this, &UInventoryWidget::RefreshInventoryUI);

    // 2. 이미 컴포넌트에 들어있는 아이템들이 있다면 한 번 싹 그려줌 (초기 동기화)
    for (const FInventoryGridItem& GridItem : CachedInvenComp->GetAllItems())
    {
        this->AddItem(GridItem.Item, GridItem.GridPos);
    }
}

int32 UInventoryWidget::ToIndex(int32 X, int32 Y) const
{
    return Y * GridWidth + X;
}

bool UInventoryWidget::CanPlaceItem(UItemBase* Item, const FIntPoint& StartPos) const
{
    if (!Item) return false;

    for (int32 Y = 0; Y < Item->ItemSize.Y; ++Y)
    {
        for (int32 X = 0; X < Item->ItemSize.X; ++X)
        {
            int32 GridX = StartPos.X + X;
            int32 GridY = StartPos.Y + Y;

            if (GridX < 0 || GridX >= GridWidth || GridY < 0 || GridY >= GridHeight)
                return false;

            int32 Index = ToIndex(GridX, GridY);
            if (Slots.IsValidIndex(Index) && Slots[Index]->IsOccupied())
                return false;
        }
    }
    return true;
}

void UInventoryWidget::SetItemOccupancy(UItemBase* Item, const FIntPoint& StartPos, bool bOccupy)
{
    if (!Item) return;

    for (int32 Y = 0; Y < Item->ItemSize.Y; ++Y)
    {
        for (int32 X = 0; X < Item->ItemSize.X; ++X)
        {
            int32 GridX = StartPos.X + X;
            int32 GridY = StartPos.Y + Y;

            int32 Index = ToIndex(GridX, GridY);
            if (Slots.IsValidIndex(Index))
                Slots[Index]->SetOccupied(bOccupy);
        }
    }
}

void UInventoryWidget::AddItem(UItemBase* Item, FIntPoint StartPos)
{
    if (!Item || !ItemWidgetClass || !CanvasPanel)
    {
        return;
    }

    // 2. 데이터 가져오기 (ItemBase 내부의 DT 데이터 조회)
    const FItemData* Data = Item->GetItemData();
    if (!Data || !Data->Icon)
    {
        UE_LOG(LogTemp, Warning, TEXT("Item Data or Icon is Missing!"));
        return;
    }

    // 3. 아이콘 위젯 생성
    UUserWidget* NewItemWidget = CreateWidget<UUserWidget>(this, ItemWidgetClass);
    if (!NewItemWidget) return;

    // 4. 이미지 셋팅 (WBP_InventoryItem 내부의 ItemIcon 이름 매칭)
    UImage* ItemIcon = Cast<UImage>(NewItemWidget->GetWidgetFromName(TEXT("ItemIcon")));
    if (ItemIcon)
    {
        ItemIcon->SetBrushFromTexture(Data->Icon); // 여기서 Data->Icon을 사용합니다.
    }

    // 5. 캔버스 배치
    UCanvasPanelSlot* ItemSlot = CanvasPanel->AddChildToCanvas(NewItemWidget);
    if (ItemSlot)
    {
        FVector2D Pos = FVector2D(StartPos.X * SlotSize.X, StartPos.Y * SlotSize.Y);
        ItemSlot->SetPosition(Pos);

        FVector2D Size = FVector2D(Item->ItemSize.X * SlotSize.X, Item->ItemSize.Y * SlotSize.Y);
        ItemSlot->SetSize(Size);

        NewItemWidget->SetVisibility(ESlateVisibility::Visible);
        ItemSlot->SetZOrder(10);
        ItemSlot->SetAnchors(FAnchors(0.0f, 0.0f));

        // [여기에 추가!] 생성한 위젯을 나중에 지울 수 있게 리스트에 담아둡니다.
        ActiveItemWidgets.Add(NewItemWidget);
    }

    // 6. 데이터 상의 점유 처리
    SetItemOccupancy(Item, StartPos, true);
}
void UInventoryWidget::RefreshInventoryUI()
{
    if (!CanvasPanel) return;

    ClearItemWidgets();

    // 2. 현재 컴포넌트에 있는 데이터를 기반으로 다시 그리기
    for (const FInventoryGridItem& GridItem : CachedInvenComp->GetAllItems())
    {
        AddItem(GridItem.Item, GridItem.GridPos);
    }
}
void UInventoryWidget::ClearItemWidgets()
{
    // 1. 현재 화면에 떠 있는 아이템 위젯들을 부모(Canvas)로부터 떼어냄
    for (UUserWidget* ItemWidget : ActiveItemWidgets)
    {
        if (ItemWidget)
        {
            ItemWidget->RemoveFromParent();
        }
    }

    // 2. 관리 리스트 비우기
    ActiveItemWidgets.Empty();
}
bool UInventoryWidget::RemoveItem(UItemBase* Item)
{
    // 제거 시 슬롯 전체 점유 해제
    for (int32 Y = 0; Y < GridHeight; ++Y)
    {
        for (int32 X = 0; X < GridWidth; ++X)
        {
            int32 Index = ToIndex(X, Y);
            if (Slots.IsValidIndex(Index) && Slots[Index]->IsOccupied())
            {
                Slots[Index]->SetOccupied(false);
            }
        }
    }
    return true;
}

void UInventoryWidget::UpdateSlot(int32 X, int32 Y, bool bOccupied)
{
    int32 Index = ToIndex(X, Y);
    if (Slots.IsValidIndex(Index))
    {
        Slots[Index]->SetOccupied(bOccupied);
    }
}

//// 만일 드래그 드랍 용 필요하다면,
//void UInventoryItemWidget::OnDragDetected()
//{
//    CachedInvenComp->NotifyItemPickedUp(MyItem);
//    SetRenderOpacity(0.5f);
//
//    UInventoryItemWidget* DragVisual = CreateWidget<UInventoryItemWidget>(this, ItemWidgetClass);
//    DragVisual->Init(MyItem);
//
//    UDragDropOperation* OutOperation = NewObject<UDragDropOperation>();
//    OutOperation->Payload = MyItem;
//    OutOperation->DefaultDragVisual = DragVisual; 
//
//    return OutOperation;
//}
//bool UInventoryWidget::OnDrop(UDragDropOperation* Operation)
//{
//    UItemBase* DroppedItem = Cast<UItemBase>(Operation->Payload);
//    FIntPoint DropGridPos = CalculateGridPos(GetMousePosition());
//    bool bSuccess = CachedInvenComp->MoveItem(DroppedItem, DropGridPos);
//
//    if (bSuccess) {
//        return true;
//    }
//    else {
//        return false;
//    }
//}
//bool UInventoryComponent::MoveItem(UItemBase* Item, FIntPoint NewPos)
//{
//
//    if (CanPlace) {
//        OnInventoryUpdated.Broadcast();
//        return true;
//    }
//    return false;
//}