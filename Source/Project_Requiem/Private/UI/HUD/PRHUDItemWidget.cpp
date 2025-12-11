#include "UI/HUD/PRHUDItemWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Kismet/KismetSystemLibrary.h"

// ========================================================
// 언리얼 기본 생성 및 초기화
// ========================================================
void UPRHUDItemWidget::NativeConstruct()
{
	if (!CurrImage) UE_LOG(LogTemp, Error, TEXT("ItemImage (Image) is not bound in UMG!"));
	if (!NewImage)  UE_LOG(LogTemp, Error, TEXT("ItemImage (Image) is not bound in UMG!"));
	if (!ItemNum)   UE_LOG(LogTemp, Error, TEXT("ItemNum (TextBlock) is not bound in UMG!"));
}
// ========================================================
// UI 변경용
// ========================================================
void UPRHUDItemWidget::ChangeItemImage(UTexture2D* NewTexture)
{
    if (!CurrImage || !NewImage || !NewTexture) return;

    PendingTexture = NewTexture;
    NewImage->SetBrushFromTexture(PendingTexture);
    NewImage->SetRenderOpacity(0.f);

    FadeAlpha = 0.0f;
    UKismetSystemLibrary::K2_SetTimer(this, TEXT("FadeTick"), 0.01f, true);
}
void UPRHUDItemWidget::ChangeItemNum(const FText& NewName)
{
	if (ItemNum) ItemNum->SetText(NewName);
}
void UPRHUDItemWidget::FadeTick()
{
    FadeAlpha += 0.05f;
    NewImage->SetRenderOpacity(FadeAlpha);
    CurrImage->SetRenderOpacity(1.0f - FadeAlpha);

    if (FadeAlpha >= 1.0f) {
        UKismetSystemLibrary::K2_ClearTimer(this, TEXT("FadeTick"));
        CurrImage->SetBrushFromTexture(PendingTexture);
        CurrImage->SetRenderOpacity(1.0f);
        NewImage->SetRenderOpacity(0.0f);
    }
}