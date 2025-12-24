#include "UI/HUD/PRHUDWidget.h"
#include "UI/HUD/PRHUDStatBarWidget.h"
#include "UI/HUD/PRHUDBossWidget.h"
#include "UI/HUD/PRHUDItemWidget.h"
#include "UI/HUD/PRHUDPotionWidget.h"

#include "Characters/Player/Character/PlayerCharacter.h"
#include "Stats/StatComponent.h"
#include "Engine/Texture2D.h"
#include "Components/TextBlock.h"

// ========================================================
// 언리얼 기본 생성 및 초기화
// ========================================================
void UPRHUDWidget::NativeConstruct()
{
    Super::NativeConstruct();
    if (!BarHP || !BarST) UE_LOG(LogTemp, Warning, TEXT("PlayerHUD: Some UMG widgets failed to bind. Check names and 'Is Variable' settings!"));

    APlayerController* PC = GetOwningPlayer();
    if (!PC) return;

    PC->GetOnNewPawnNotifier().AddUObject(this, &UPRHUDWidget::OnPawnChanged);
    if (APawn* CurrentPawn = PC->GetPawn()) {
        OnPawnChanged(CurrentPawn);
        Player = Cast<APlayerCharacter>(CurrentPawn);
        if (Player)
        {
            Player->OnInteractionPromptChanged.AddDynamic(this, &UPRHUDWidget::UpdateActionText);
        }
    }

    if (BarHP) {
        BarHP->SetBarColor(FLinearColor::Red);
        BarHP->SetStatName(FText::FromString(TEXT("HP")));
    }
    if (BarST) {
        BarST->SetBarColor(FLinearColor::Yellow);
        BarST->SetStatName(FText::FromString(TEXT("ST")));
    }
    if (BossBarHP) {
        BossBarHP->SetVisibility(ESlateVisibility::Hidden);
        BossBarHP->SetBarColor(FLinearColor::Red);
        BossBarHP->SetStatName(FText::FromString(TEXT("BossName")));
    }
    if (WeaponSlot) {
        WeaponSlot->ChangeItemImage(ImageSlot0);
    }
    if (PotionSlot && PotionImage) {
        PotionSlot->ItitItemImage(PotionImage);
        Player->OnPotionChanged.AddDynamic(this, &UPRHUDWidget::UpdatePotionNum);
        UpdatePotionNum(Player->GetHPPotion());
    }
    if (ActionTextValue) {
        ActionTextValue->SetVisibility(ESlateVisibility::Hidden);
    }
}
void UPRHUDWidget::BindToPawn(APawn* NewPawn)
{
    if (UStatComponent* StatComp = NewPawn->FindComponentByClass<UStatComponent>()) {
        // 스탯 리젠용 제거
        StatComp->OnRegenStatChanged.RemoveDynamic(this, &UPRHUDWidget::HandleRegenStatChanged);

        // 스탯 리젠용 다시 붙이기
        StatComp->OnRegenStatChanged.AddDynamic(this, &UPRHUDWidget::HandleRegenStatChanged);
    }
}
void UPRHUDWidget::OnPawnChanged(APawn* NewPawn)
{
    if (NewPawn) BindToPawn(NewPawn);
}
// ========================================================
// 스탯 바 갱신
// ========================================================
void UPRHUDWidget::HandleRegenStatChanged(EFullStats StatType, float CurrValue, float MaxValue)
{
    switch (StatType) {
    case EFullStats::Health:
        UpdateHPBar(CurrValue, MaxValue);
        break;
    case EFullStats::Stamina:
        UpdateSTBar(CurrValue, MaxValue);
        break;

    default:
        break;
    }
}
void UPRHUDWidget::UpdateHPBar(float CurrentValue, float MaxValue)
{
    if (BarHP) BarHP->SetStatValue(CurrentValue, MaxValue);
}
void UPRHUDWidget::UpdateSTBar(float CurrentValue, float MaxValue)
{
    if (BarST) BarST->SetStatValue(CurrentValue, MaxValue);
}
// ========================================================
// 보스 HP바 갱신
// ========================================================
void UPRHUDWidget::ShowBossHPBar(bool bShow)
{
    if (!BossBarHP) return;
    BossBarHP->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}
void UPRHUDWidget::SetBossName(const FText& Name)
{
    if (!BossBarHP) return;    
    BossBarHP->SetStatName(Name);
}
void UPRHUDWidget::HandleBossStatChanged(EFullStats StatType, float CurrValue, float MaxValue)
{
    switch (StatType) {
    case EFullStats::Health:
        UpdateBossHPBar(CurrValue, MaxValue);
        break;
    default:
        // HP만 갱신함
        break;
    }
}
void UPRHUDWidget::UpdateBossHPBar(float CurrentValue, float MaxValue)
{
    if (BossBarHP) BossBarHP->SetStatValue(CurrentValue, MaxValue);
}
// ========================================================
// 무기 아이템 스왑용
// ========================================================
void UPRHUDWidget::UpdateWeaponSlot(int32 SlotIndex)
{
    switch (SlotIndex) {
    case 0:
        WeaponSlot->ChangeItemImage(ImageSlot0);
        break;
    case 1:
        WeaponSlot->ChangeItemImage(ImageSlot1);
        break;
    case 2:
        WeaponSlot->ChangeItemImage(ImageSlot2);
        break;
    default:
        UE_LOG(LogTemp, Warning, TEXT("UpdateWeaponSlot: Invalid SlotIndex %d"), SlotIndex);
        break;
    }

}
// ========================================================
// 포션용 위젯
// ========================================================
void UPRHUDWidget::UpdatePotionNum(int32 ItemNum)
{
    if (PotionSlot) {
        FText ItemNumText = FText::AsNumber(ItemNum);
        PotionSlot->ChangeItemNum(ItemNumText);
    }
}
// ========================================================
// 인터렉션
// ========================================================
void UPRHUDWidget::UpdateActionText(const FText& Text)
{
    if (Text.IsEmpty()) ActionTextValue->SetVisibility(ESlateVisibility::Hidden);
    else {
        ActionTextValue->SetText(Text);
        ActionTextValue->SetVisibility(ESlateVisibility::Visible);
    }
}