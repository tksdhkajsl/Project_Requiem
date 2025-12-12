#include "Core/PRPlayerController.h"

#include "UI/HUD/PRHUDWidget.h"
// ========================================================
// �𸮾� �⺻����
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
// HUD ���� Ŭ����
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