#include "UI/HUD/PRHUDBossWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

// ========================================================
// 언리얼 기본 생성
// ========================================================
void UPRHUDBossWidget::NativeConstruct()
{
    if (!StatValue) UE_LOG(LogTemp, Error, TEXT("StatValue (ProgressBar) is not bound in UMG!"));
    if (!StatName)  UE_LOG(LogTemp, Error, TEXT("StatName (TextBlock) is not bound in UMG!"));
}
// ========================================================
// UI 변경용
// ========================================================
void UPRHUDBossWidget::SetStatValue(float CurrentValue, float MaxValue)
{
    if (StatValue) {
        float Percent = (MaxValue > 0) ? (CurrentValue / MaxValue) : 0.0f;
        StatValue->SetPercent(Percent);
    }
}
void UPRHUDBossWidget::SetStatName(const FText& NewName)
{
    if (StatName) StatName->SetText(NewName);
}
void UPRHUDBossWidget::SetBarColor(FLinearColor NewColor)
{
    if (StatValue) StatValue->SetFillColorAndOpacity(NewColor);
}