#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/Inventory/InventorySlotWidget.h"
#include "InventoryWidget.generated.h"

class UCanvasPanel;
class UItemBase;
class UInventoryComponent;

UCLASS()
class PROJECT_REQUIEM_API UInventoryWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    /** 슬롯 상태 갱신 (점유 여부) */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void UpdateSlot(int32 X, int32 Y, bool bOccupied);

    /** 아이템 추가 (ItemSize 고려) */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void AddItem(UItemBase* Item, FIntPoint StartPos);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void RefreshInventoryUI();
    UFUNCTION()
    void ClearItemWidgets();

    /** 아이템 제거 */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool RemoveItem(UItemBase* Item);
    UFUNCTION()
    void InitInventory(UInventoryComponent* InComp);
protected:
    /** 슬롯을 담는 패널 */
    UPROPERTY(meta = (BindWidget))
    UCanvasPanel* CanvasPanel;

    UPROPERTY(EditDefaultsOnly, Category = "Inventory")
    TSubclassOf<UUserWidget> ItemWidgetClass;

    /** 슬롯 위젯 클래스 */
    UPROPERTY(EditDefaultsOnly, Category = "Inventory")
    TSubclassOf<UInventorySlotWidget> SlotClass;

    /** 슬롯 크기 (UI 픽셀 기준) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
    FVector2D SlotSize = FVector2D(64.f, 64.f);

    /** 가로/세로 슬롯 수 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
    int32 GridWidth = 6;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
    int32 GridHeight = 6;

    /** 생성된 슬롯 위젯 배열 */
    UPROPERTY()
    TArray<UInventorySlotWidget*> Slots;

protected:
    /** 슬롯 생성 */
    void CreateSlots();


    UPROPERTY()
    UInventoryComponent* CachedInvenComp;



    /** X,Y 좌표를 인덱스로 변환 */
    int32 ToIndex(int32 X, int32 Y) const;

    /** 특정 영역에 아이템 놓을 수 있는지 */
    bool CanPlaceItem(UItemBase* Item, const FIntPoint& StartPos) const;

    /** 특정 영역 점유/해제 */
    void SetItemOccupancy(UItemBase* Item, const FIntPoint& StartPos, bool bOccupy);

    UPROPERTY()
    TArray<UUserWidget*> ActiveItemWidgets;
};