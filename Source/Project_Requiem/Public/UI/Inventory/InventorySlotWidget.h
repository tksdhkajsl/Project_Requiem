#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySlotWidget.generated.h"

class UImage;
class UTexture2D;

UCLASS()
class PROJECT_REQUIEM_API UInventorySlotWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

public:
    /** 점유 상태 갱신 (인벤토리 위젯에서 호출) */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void SetOccupied(bool bInOccupied);

    UFUNCTION()
    bool IsOccupied() { return bOccupied; }

protected:
    /** 슬롯 배경 이미지 */
    UPROPERTY(meta = (BindWidget))
    UImage* SlotImage;

    /** 점유 상태 텍스처 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
    UTexture2D* OccupiedTexture;

    /** 미점유 상태 텍스처 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
    UTexture2D* UnoccupiedTexture;

    /** 현재 슬롯이 점유 중인가 (비주얼 전용 상태) */
    UPROPERTY(BlueprintReadOnly, Category = "Inventory")
    bool bOccupied = false;

protected:
    /** 점유 상태에 따른 색상 갱신 */
    void UpdateVisual();
};