#include "Core/PRPlayerController.h"

#include "UI/HUD/PRHUDWidget.h"
// ========================================================
// 언리얼 기본생성
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
// ========================================================
// HUD 위젯 클래스
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