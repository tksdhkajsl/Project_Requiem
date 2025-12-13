#include "UI/StatWidget/PRStatRenderWidget.h"
#include "Components/TextBlock.h"

// ========================================================
// 언리얼 기본 생성
// ========================================================
void UPRStatRenderWidget::NativeConstruct()
{
	if (!HealthValueText)       UE_LOG(LogTemp, Error, TEXT("HealthValueText (TextBlock) is not bound in UMG!"));
    if (!MaxHealthValueText)    UE_LOG(LogTemp, Error, TEXT("MaxHealthValueText (TextBlock) is not bound in UMG!"));
    if (!StaminaValueText)      UE_LOG(LogTemp, Error, TEXT("StaminaValueText (TextBlock) is not bound in UMG!"));
    if (!MaxStaminaValueText)   UE_LOG(LogTemp, Error, TEXT("MaxStaminaValueText (TextBlock) is not bound in UMG!"));
    if (!PhysicalAttackText)    UE_LOG(LogTemp, Error, TEXT("PhysicalAttackText (TextBlock) is not bound in UMG!"));
    if (!MagicAttackText)       UE_LOG(LogTemp, Error, TEXT("MagicAttackText (TextBlock) is not bound in UMG!"));
    if (!PhysicalDefenseText)   UE_LOG(LogTemp, Error, TEXT("PhysicalDefenseText (TextBlock) is not bound in UMG!"));
    if (!MagicDefenseText)      UE_LOG(LogTemp, Error, TEXT("MagicDefenseText (TextBlock) is not bound in UMG!"));
}
// ========================================================
// 스탯 갱신
// ========================================================
void UPRStatRenderWidget::UpdateResourceStat(EFullStats StatType, float CurrentValue, float MaxValue)
{
    float Percent = FMath::Clamp(CurrentValue / MaxValue, 0.0f, 1.0f);
    FString CurrentValueString = FString::FromInt(FMath::RoundToInt(CurrentValue));
    FString MaxValueString = FString::FromInt(FMath::RoundToInt(MaxValue));

    switch (StatType) {
    case EFullStats::Health:
        if (HealthValueText)    HealthValueText->SetText(FText::FromString(CurrentValueString));
        if (MaxHealthValueText) MaxHealthValueText->SetText(FText::FromString(MaxValueString));
        break;

    case EFullStats::Stamina:
        if (StaminaValueText)   StaminaValueText->SetText(FText::FromString(CurrentValueString));
        if (MaxStaminaValueText)MaxStaminaValueText->SetText(FText::FromString(MaxValueString));
        break;

    default:
        break;
    }
}
void UPRStatRenderWidget::UpdateSingleStat(EFullStats StatType, float Value)
{
    FString ValueString = FString::FromInt(FMath::RoundToInt(Value));

    switch (StatType) {
    case EFullStats::PhysicalAttack:
        if (PhysicalAttackText) PhysicalAttackText->SetText(FText::FromString(ValueString));
        break;

    case EFullStats::MagicAttack:
        if (MagicAttackText) MagicAttackText->SetText(FText::FromString(ValueString));
        break;

    case EFullStats::PhysicalDefense:
        if (PhysicalDefenseText) PhysicalDefenseText->SetText(FText::FromString(ValueString));
        break;

    case EFullStats::MagicDefense:
        if (MagicDefenseText) MagicDefenseText->SetText(FText::FromString(ValueString));
        break;

    default:
        break;
    }
}