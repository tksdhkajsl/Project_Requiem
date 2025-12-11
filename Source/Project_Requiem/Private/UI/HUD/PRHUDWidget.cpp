#include "UI/HUD/PRHUDWidget.h"
#include "UI/HUD/PRHUDStatBarWidget.h"
#include "UI/HUD/PRHUDBossWidget.h"
#include "UI/HUD/PRHUDItemWidget.h"
#include "UI/HUD/PRHUDPotionWidget.h"

#include "Stats/StatComponent.h"
#include "Engine/Texture2D.h"

// ========================================================
// 언리얼 기본 생성 및 초기화
// ========================================================
void UPRHUDWidget::NativeConstruct()
{
    Super::NativeConstruct();
    if (!BarHP || !BarST) UE_LOG(LogTemp, Warning, TEXT("PlayerHUD: Some UMG widgets failed to bind. Check names and 'Is Variable' settings!"));

    // StatBar의 초기 색상 및 글자 설정 (이후로 변경 안함)
    if (BarHP) {
        BarHP->SetBarColor(FLinearColor::Red);
        BarHP->SetStatName(FText::FromString(TEXT("HP")));
    }
    if (BarST) {
        BarST->SetBarColor(FLinearColor::Yellow);
        BarST->SetStatName(FText::FromString(TEXT("ST")));
    }
    if (BossBarHP) {
        BossBarHP->SetBarColor(FLinearColor::Red);
        BossBarHP->SetStatName(FText::FromString(TEXT("BossName")));
    }
    if (WeaponSlot) {
        WeaponSlot->ChangeItemImage(ImageSlot0);
    }
    if (PotionSlot && PotionImage) {
        PotionSlot->ItitItemImage(PotionImage);
    }

    APlayerController* PC = GetOwningPlayer();
    if (!PC) return;

    PC->GetOnNewPawnNotifier().AddUObject(this, &UPRHUDWidget::OnPawnChanged);
    if (APawn* CurrentPawn = PC->GetPawn()) OnPawnChanged(CurrentPawn);
}
void UPRHUDWidget::BindToPawn(APawn* NewPawn)
{
    if (UStatComponent* StatComp = NewPawn->FindComponentByClass<UStatComponent>()) {
        // 중복 바인딩 방지
        StatComp->OnRegenStatChanged.RemoveDynamic(this, &UPRHUDWidget::HandleRegenStatChanged);

        // 다시 바인딩
        StatComp->OnRegenStatChanged.AddDynamic(this, &UPRHUDWidget::HandleRegenStatChanged);
    }
}
void UPRHUDWidget::OnPawnChanged(APawn* NewPawn)
{
    if (NewPawn) BindToPawn(NewPawn);
}
// ========================================================
// 언리얼 기본 생성 및 초기화
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
        // 기타 처리 (필요시)
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
void UPRHUDWidget::HandleBossStatChanged(EFullStats StatType, float CurrValue, float MaxValue)
{
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
