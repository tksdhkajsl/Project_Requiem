#include "Core/PRPlayerController.h"
#include "UI/HUD/PRHUDWidget.h"

#include "Character/BaseCharacter.h"
#include "Stats/StatComponent.h"
// ========================================================
// 언리얼 기본 생성
// ========================================================
APRPlayerController::APRPlayerController()
{
}
void APRPlayerController::BeginPlay()
{
	if (HUDWidgetClass) {
		PlayerHUD = CreateWidget<UPRHUDWidget>(this, HUDWidgetClass);
		if (PlayerHUD) {
			PlayerHUD->AddToViewport();
		}
	}
}
void APRPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (ActiveBoss && ActiveBoss->GetStatComponent()) {
		ActiveBoss->GetStatComponent()->OnRegenStatChanged.RemoveDynamic(this, &APRPlayerController::HandleBossHPChanged);
	}
	Super::EndPlay(EndPlayReason);
}
// ========================================================
// 아이템 위젯 클래스
// ========================================================
void APRPlayerController::PushDownKeyboard1()
{
	PlayerHUD->UpdateWeaponSlot(0);
}
void APRPlayerController::PushDownKeyboard2()
{
	PlayerHUD->UpdateWeaponSlot(1);
}
void APRPlayerController::PushDownKeyboard3()
{
	PlayerHUD->UpdateWeaponSlot(2);
}
void APRPlayerController::PushDownKeyboard4(int32 PotionNum)
{
	PlayerHUD->UpdatePotionNum(PotionNum);
}
// ========================================================
// 보스 관련 HUD 갱신
// ========================================================
void APRPlayerController::OnEnterBossRoom(ABaseCharacter* Boss)
{
	if (!PlayerHUD || !Boss) return;

	// 보스 이름 설정
	PlayerHUD->SetBossName(Boss->MonsterName);

	// 초기 체력 갱신
	if (UStatComponent* StatComp = Boss->GetStatComponent()) {
		PlayerHUD->UpdateBossHPBar(StatComp->CurrHP, StatComp->MaxHP);

		StatComp->OnRegenStatChanged.RemoveDynamic(this, &APRPlayerController::HandleBossHPChanged);
		StatComp->OnRegenStatChanged.AddDynamic(this, &APRPlayerController::HandleBossHPChanged);
	}

	PlayerHUD->ShowBossHPBar(true);
}
void APRPlayerController::HandleBossHPChanged(EFullStats StatType, float Curr, float Max)
{
	if (!PlayerHUD) return;
	if (StatType != EFullStats::Health) return;

	PlayerHUD->UpdateBossHPBar(Curr, Max);

	// 죽으면 숨기기
	if (Curr <= 0.f) {
		PlayerHUD->ShowBossHPBar(false);
		if (ActiveBoss && ActiveBoss->GetStatComponent()) {
			ActiveBoss->GetStatComponent()->OnRegenStatChanged.RemoveDynamic(this, &APRPlayerController::HandleBossHPChanged);
		}
		ActiveBoss = nullptr;
	}
}