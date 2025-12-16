#include "UI/StatWidget/PRStatWidget.h"
#include "UI/StatWidget/PRPlayerPortraitWidget.h"
#include "UI/StatWidget/PRStatRenderWidget.h"

#include "Stats/StatComponent.h"
#include "Stats/Data/EFullStats.h"

// ========================================================
// 언리얼 기본 생성
// ========================================================
void UPRStatWidget::NativeConstruct()
{
}
void UPRStatWidget::NativeDestruct()
{
    if (CachedStatComponent) {
        CachedStatComponent->OnRegenStatChanged.RemoveDynamic(this, &UPRStatWidget::HandleRegenStatUpdate);
    }

    Super::NativeDestruct();
}
// ========================================================
// 스탯컴포넌트 캐싱용
// ========================================================
void UPRStatWidget::SetStatComponent(UStatComponent* Component)
{
    if (CachedStatComponent) {
        CachedStatComponent->OnRegenStatChanged.RemoveDynamic(this, &UPRStatWidget::HandleRegenStatUpdate);
    }

    CachedStatComponent = Component;
    if (CachedStatComponent && StatRenderList) {
        CachedStatComponent->OnRegenStatChanged.AddDynamic(this, &UPRStatWidget::HandleRegenStatUpdate);
        InitializeStatValues();
    }
}
void UPRStatWidget::InitializeStatValues()
{
    if (!CachedStatComponent || !StatRenderList) return;

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
void UPRStatWidget::HandleRegenStatUpdate(EFullStats StatType, float CurrentValue, float MaxValue)
{
    if (StatRenderList) StatRenderList->UpdateResourceStat(StatType, CurrentValue, MaxValue);
}
