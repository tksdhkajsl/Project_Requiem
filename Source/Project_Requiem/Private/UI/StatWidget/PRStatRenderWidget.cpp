#include "UI/StatWidget/PRStatRenderWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

// ========================================================
// 언리얼 기본 생성
// ========================================================
void UPRStatRenderWidget::NativeConstruct()
{
	if (!StrValueText)          UE_LOG(LogTemp, Error, TEXT("StrValueText (TextBlock) is not bound in UMG!"));
	if (!HealthValueText)       UE_LOG(LogTemp, Error, TEXT("HealthValueText (TextBlock) is not bound in UMG!"));
    if (!MaxHealthValueText)    UE_LOG(LogTemp, Error, TEXT("MaxHealthValueText (TextBlock) is not bound in UMG!"));
    if (!PhysicalAttackText)    UE_LOG(LogTemp, Error, TEXT("PhysicalAttackText (TextBlock) is not bound in UMG!"));
    
    if (!DexValueText)          UE_LOG(LogTemp, Error, TEXT("DexValueText (TextBlock) is not bound in UMG!"));
    if (!StaminaValueText)      UE_LOG(LogTemp, Error, TEXT("StaminaValueText (TextBlock) is not bound in UMG!"));
    if (!MaxStaminaValueText)   UE_LOG(LogTemp, Error, TEXT("MaxStaminaValueText (TextBlock) is not bound in UMG!"));
    if (!AttackSpeedText)       UE_LOG(LogTemp, Error, TEXT("AttackSpeedText (TextBlock) is not bound in UMG!"));

    if (!MagicAttackText)       UE_LOG(LogTemp, Error, TEXT("MagicAttackText (TextBlock) is not bound in UMG!"));
    if (!PhysicalDefenseText)   UE_LOG(LogTemp, Error, TEXT("PhysicalDefenseText (TextBlock) is not bound in UMG!"));
    if (!MagicDefenseText)      UE_LOG(LogTemp, Error, TEXT("MagicDefenseText (TextBlock) is not bound in UMG!"));

    if (StrIncreaseButton) {
        StrIncreaseButton->OnClicked.RemoveAll(this);
        StrIncreaseButton->OnClicked.AddDynamic(this, &UPRStatRenderWidget::OnClick_StrIncrease);
    }
    if (DexIncreaseButton) {
        DexIncreaseButton->OnClicked.RemoveAll(this);
        DexIncreaseButton->OnClicked.AddDynamic(this, &UPRStatRenderWidget::OnClick_DexIncrease);
    }
}
void UPRStatRenderWidget::NativeDestruct()
{
    if (StrIncreaseButton) StrIncreaseButton->OnClicked.RemoveAll(this);
    if (DexIncreaseButton) DexIncreaseButton->OnClicked.RemoveAll(this);

    Super::NativeDestruct();
}
// ========================================================
// 기본 스탯 갱신
// ========================================================
void UPRStatRenderWidget::UpdateLevelUpStat(ELevelUpStats StatType, int32 AllocatedPoints)
{
    switch (StatType)
    {
    case ELevelUpStats::Strength:
        if (StrValueText) StrValueText->SetText(FText::AsNumber(AllocatedPoints));
        break;
    case ELevelUpStats::Dexterity:
        if (DexValueText) DexValueText->SetText(FText::AsNumber(AllocatedPoints));
        break;
    default:
        break;
    }
    
}
void UPRStatRenderWidget::HiddenButton()
{
    if (StrValueText) StrIncreaseButton->SetVisibility(ESlateVisibility::Hidden);
    if (DexValueText) DexIncreaseButton->SetVisibility(ESlateVisibility::Hidden);
}
void UPRStatRenderWidget::ShowButton()
{
    if (StrIncreaseButton) StrIncreaseButton->SetVisibility(ESlateVisibility::Visible);
    if (DexIncreaseButton) DexIncreaseButton->SetVisibility(ESlateVisibility::Visible);
}
void UPRStatRenderWidget::OnClick_StrIncrease()
{
    OnRequestLevelUpStat.Broadcast(ELevelUpStats::Strength);
}
void UPRStatRenderWidget::OnClick_DexIncrease()
{
    OnRequestLevelUpStat.Broadcast(ELevelUpStats::Dexterity);
}
// ========================================================
// 파생 스탯 갱신
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
    case EFullStats::AttackSpeed:
        if (AttackSpeedText) AttackSpeedText->SetText(FText::FromString(ValueString));
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