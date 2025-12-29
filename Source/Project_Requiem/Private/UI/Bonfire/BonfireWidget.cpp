#include "UI/Bonfire/BonfireWidget.h"
#include "UI/StatWidget/PRStatRenderWidget.h"

#include "Components/TextBlock.h"
#include "Stats/StatComponent.h"
#include "Stats/Data/ELevelUpStats.h"
#include "Stats/Data/EFullStats.h"

#include "Characters/Player/Character/PlayerCharacter.h"
#include "Components/Button.h"

// ========================================================
// 언리얼 기본 생성
// ========================================================
void UBonfireWidget::NativeConstruct()
{
    if (!SoulValueText) UE_LOG(LogTemp, Error, TEXT("StrValueText (TextBlock) is not bound in UMG!"));
    if (CloseBonfireButton) CloseBonfireButton->OnClicked.AddDynamic(this, &UBonfireWidget::CloseBonfire);

    if (StatRenderList) {
        StatRenderList->OnRequestLevelUpStat.RemoveAll(this);
        StatRenderList->OnRequestLevelUpStat.AddDynamic(this, &UBonfireWidget::HandleRequestLevelUpStat);
    }

    if (APlayerController* PC = GetOwningPlayer()) PlayerRef = Cast<APlayerCharacter>(PC->GetPawn());
    if (BuyPotionButton) BuyPotionButton->OnClicked.AddDynamic(this, &UBonfireWidget::OnBuyPotionClicked);

    if (APlayerController* PC = GetOwningPlayer()) {
        FInputModeUIOnly InputMode;
        InputMode.SetWidgetToFocus(TakeWidget());
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = true;
    }

    //IsOpen = true;
}
void UBonfireWidget::NativeDestruct()
{
    if (CachedStatComponent) {
        CachedStatComponent->OnLevelUpStatsUpdated.RemoveDynamic(this, &UBonfireWidget::HandleBasicStatUpdate);
        CachedStatComponent->OnRegenStatChanged.RemoveDynamic(this, &UBonfireWidget::HandleRegenStatUpdate);
        CachedStatComponent->OnExpChanged.RemoveAll(this);
    }
    if (StatRenderList) StatRenderList->OnRequestLevelUpStat.RemoveAll(this);
    if (BuyPotionButton) BuyPotionButton->OnClicked.RemoveDynamic(this, &UBonfireWidget::OnBuyPotionClicked);
    if (CloseBonfireButton) CloseBonfireButton->OnClicked.RemoveDynamic(this, &UBonfireWidget::CloseBonfire);

    Super::NativeDestruct();
}
// ========================================================
// 위젯 닫기 버튼
// ========================================================
void UBonfireWidget::CloseBonfire()
{
    //if (!IsOpen) return;
    //IsOpen = false;

    RemoveFromParent();
    if (APlayerController* PC = GetOwningPlayer()) {
        FInputModeGameOnly InputMode;
        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = false;

        OnBonfireClosed.Broadcast();
    }
}
// ========================================================
// 포션 상점
// ========================================================
void UBonfireWidget::OnBuyPotionClicked()
{
    if (!PlayerRef && !CachedStatComponent) return;

    if (CachedStatComponent->GetCurrentExp() >= PotionPrice) {
        PlayerRef->AddPotion();
        CachedStatComponent->AddCurrentExp(-PotionPrice);
    }
    else UE_LOG(LogTemp, Warning, TEXT("소울이 부족합니다!"));

}
// ========================================================
// 스탯컴포넌트 캐싱용
// ========================================================
void UBonfireWidget::SetStatComponent(UStatComponent* Component)
{
    if (CachedStatComponent) {
        CachedStatComponent->OnLevelUpStatsUpdated.RemoveDynamic(this, &UBonfireWidget::HandleBasicStatUpdate);
        CachedStatComponent->OnRegenStatChanged.RemoveDynamic(this, &UBonfireWidget::HandleRegenStatUpdate);
        CachedStatComponent->OnExpChanged.RemoveDynamic(this, &UBonfireWidget::HandleExpChanged);
    }

    CachedStatComponent = Component;
    if (CachedStatComponent && StatRenderList) {
        CachedStatComponent->OnLevelUpStatsUpdated.AddDynamic(this, &UBonfireWidget::HandleBasicStatUpdate);
        CachedStatComponent->OnRegenStatChanged.AddDynamic(this, &UBonfireWidget::HandleRegenStatUpdate);
        CachedStatComponent->OnExpChanged.AddDynamic(this, &UBonfireWidget::HandleExpChanged);
        InitializeStatValues();
    }
}
void UBonfireWidget::InitializeStatValues()
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
    // [추가] 12/29, 공격 속도가 표기가 안 되어 있었음
    float AttSpeed = CachedStatComponent->GetStatCurrent(EFullStats::AttackSpeed);
    StatRenderList->UpdateSingleStat(EFullStats::AttackSpeed, AttSpeed);
    float MagAtt = CachedStatComponent->GetStatCurrent(EFullStats::MagicAttack);
    StatRenderList->UpdateSingleStat(EFullStats::MagicAttack, MagAtt);
    float PhyDef = CachedStatComponent->GetStatCurrent(EFullStats::PhysicalDefense);
    StatRenderList->UpdateSingleStat(EFullStats::PhysicalDefense, PhyDef);
    float MagDef = CachedStatComponent->GetStatCurrent(EFullStats::MagicDefense);
    StatRenderList->UpdateSingleStat(EFullStats::MagicDefense, MagDef);

}
void UBonfireWidget::HandleBasicStatUpdate(ELevelUpStats StatType, int32 AllocatedPoints)
{
    if (StatRenderList) StatRenderList->UpdateLevelUpStat(StatType, AllocatedPoints);
}
void UBonfireWidget::HandleRegenStatUpdate(EFullStats StatType, float CurrentValue, float MaxValue)
{
    if (StatRenderList) StatRenderList->UpdateResourceStat(StatType, CurrentValue, MaxValue);
}
void UBonfireWidget::HandleExpChanged(float CurrentExp)
{
    if (StatRenderList) SoulValueText->SetText(FText::AsNumber(FMath::FloorToInt(CurrentExp)));
}
void UBonfireWidget::HandleRequestLevelUpStat(ELevelUpStats StatType)
{
    if (!CachedStatComponent) return;

    if (CachedStatComponent->AllocatedLevelUpStat(StatType)) {
        UE_LOG(LogTemp, Log, TEXT("Level Up Success! Remaining Exp: %.0f"), CachedStatComponent->GetCurrentExp());

        // [추가] 12/29, 스탯 투자가 되면 즉시 UI 전체 값을 갱신하여 변화를 보여줌
        InitializeStatValues();
    }
    else UE_LOG(LogTemp, Warning, TEXT("Not Enough Souls!"));
}
