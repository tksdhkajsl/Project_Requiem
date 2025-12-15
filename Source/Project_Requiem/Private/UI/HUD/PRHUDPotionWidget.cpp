#include "UI/HUD/PRHUDPotionWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
// ========================================================
// 언리얼 기본 생성
// ========================================================
void UPRHUDPotionWidget::ItitItemImage(UTexture2D* NewTexture)
{
    ItemImage->SetBrushFromTexture(NewTexture);
}
void UPRHUDPotionWidget::NativeConstruct()
{
    if (!ItemImage)  UE_LOG(LogTemp, Error, TEXT("ItemImage (Image) is not bound in UMG!"));
    if (!ItemNumText)   UE_LOG(LogTemp, Error, TEXT("ItemNum (TextBlock) is not bound in UMG!"));
}
// ========================================================
// UI 변경용
// ========================================================
void UPRHUDPotionWidget::ChangeItemNum(const FText& NewName)
{
    if (ItemNumText) ItemNumText->SetText(NewName);
}