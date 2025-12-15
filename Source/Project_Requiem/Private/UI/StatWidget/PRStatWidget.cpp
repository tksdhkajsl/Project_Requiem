#include "UI/StatWidget/PRStatWidget.h"
#include "UI/StatWidget/PRStatRenderWidget.h"

#include "Components/TextBlock.h"
#include "Stats/StatComponent.h"
#include "Stats/Data/ELevelUpStats.h"
#include "Stats/Data/EFullStats.h"

// ========================================================
// 언리얼 기본 생성
// ========================================================
void UPRStatWidget::NativeConstruct()
{
    if (!SoulValueText) UE_LOG(LogTemp, Error, TEXT("StrValueText (TextBlock) is not bound in UMG!"));

    if (StatRenderList) {
        StatRenderList->OnRequestLevelUpStat.RemoveAll(this);
        StatRenderList->OnRequestLevelUpStat.AddDynamic(this, &UPRStatWidget::HandleRequestLevelUpStat);
    }
}
void UPRStatWidget::NativeDestruct()
{
    if (CachedStatComponent) {
        CachedStatComponent->OnLevelUpStatsUpdated.RemoveDynamic(this, &UPRStatWidget::HandleBasicStatUpdate);
        CachedStatComponent->OnRegenStatChanged.RemoveDynamic(this, &UPRStatWidget::HandleRegenStatUpdate);
        CachedStatComponent->OnExpChanged.RemoveAll(this);
    }
    if (StatRenderList) StatRenderList->OnRequestLevelUpStat.RemoveAll(this);

    Super::NativeDestruct();
}
// ========================================================
// 스탯컴포넌트 캐싱용
// ========================================================
void UPRStatWidget::SetStatComponent(UStatComponent* Component)
{
    if (CachedStatComponent) {
        CachedStatComponent->OnLevelUpStatsUpdated.RemoveDynamic(this, &UPRStatWidget::HandleBasicStatUpdate);
        CachedStatComponent->OnRegenStatChanged.RemoveDynamic(this, &UPRStatWidget::HandleRegenStatUpdate);
        CachedStatComponent->OnExpChanged.RemoveDynamic(this, &UPRStatWidget::HandleExpChanged);
    }

    CachedStatComponent = Component;
    if (CachedStatComponent && StatRenderList) {
        CachedStatComponent->OnLevelUpStatsUpdated.AddDynamic(this, &UPRStatWidget::HandleBasicStatUpdate);
        CachedStatComponent->OnRegenStatChanged.AddDynamic(this, &UPRStatWidget::HandleRegenStatUpdate);
        CachedStatComponent->OnExpChanged.AddDynamic(this, &UPRStatWidget::HandleExpChanged);
        InitializeStatValues();
    }
}
void UPRStatWidget::InitializeStatValues()
{
    if (!CachedStatComponent || !StatRenderList) return;

    float CurrentExp = CachedStatComponent->GetCurrentExp();
    FString ExpString = FString::Printf(TEXT("%.0f"), CurrentExp); // 소수점 없이 정수로 표시
    SoulValueText->SetText(FText::FromString(ExpString));

    //StatRenderList->HiddenButton();
    StatRenderList->UpdateLevelUpStat(ELevelUpStats::Strength, CachedStatComponent->LevelUpStats.GetAllocatedPoint(ELevelUpStats::Strength));
    StatRenderList->UpdateLevelUpStat(ELevelUpStats::Dexterity, CachedStatComponent->LevelUpStats.GetAllocatedPoint(ELevelUpStats::Dexterity));

    float HealthMax = CachedStatComponent->GetStatMax(EFullStats::Health);
    float HealthCurrent = CachedStatComponent->GetStatCurrent(EFullStats::Health);
    StatRenderList->UpdateResourceStat(EFullStats::Health, HealthCurrent, HealthMax);

    float StaminaMax = CachedStatComponent->GetStatMax(EFullStats::Stamina);
    float StaminaCurrent = CachedStatComponent->GetStatCurrent(EFullStats::Stamina);
    StatRenderList->UpdateResourceStat(EFullStats::Stamina, StaminaCurrent, StaminaMax);

    float PhyAtt = CachedStatComponent->GetStatCurrent(EFullStats::PhysicalAttack);
    StatRenderList->UpdateSingleStat(EFullStats::PhysicalAttack, PhyAtt);
    float MagAtt = CachedStatComponent->GetStatCurrent(EFullStats::MagicAttack);
    StatRenderList->UpdateSingleStat(EFullStats::MagicAttack, MagAtt);
    float PhyDef = CachedStatComponent->GetStatCurrent(EFullStats::PhysicalDefense);
    StatRenderList->UpdateSingleStat(EFullStats::PhysicalDefense, PhyDef);
    float MagDef = CachedStatComponent->GetStatCurrent(EFullStats::MagicDefense);
    StatRenderList->UpdateSingleStat(EFullStats::MagicDefense, MagDef);
    
}
void UPRStatWidget::HandleBasicStatUpdate(ELevelUpStats StatType, int32 AllocatedPoints)
{
    if (StatRenderList) StatRenderList->UpdateLevelUpStat(StatType, AllocatedPoints);
}
void UPRStatWidget::HandleRegenStatUpdate(EFullStats StatType, float CurrentValue, float MaxValue)
{
    if (StatRenderList) StatRenderList->UpdateResourceStat(StatType, CurrentValue, MaxValue);
}
void UPRStatWidget::HandleExpChanged(float CurrentExp)
{
    if (StatRenderList) SoulValueText->SetText(FText::AsNumber(FMath::FloorToInt(CurrentExp)));
}
void UPRStatWidget::HandleRequestLevelUpStat(ELevelUpStats StatType)
{
    if (!CachedStatComponent) return;

    CachedStatComponent->AllocatedLevelUpStat(StatType);
}
