#include "Core/PRPlayerController.h"
#include "UI/HUD/PRHUDWidget.h"
#include "UI/StatWidget/PRStatWidget.h"
//
#include "Characters/BaseCharacter.h"
#include "Characters/Player/Character/PlayerCharacter.h"
#include "Stats/StatComponent.h"
#include "Components/SceneCaptureComponent2D.h"
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

	if (StatWidgetClass) {
		PlayerStatWidget = CreateWidget<UPRStatWidget>(this, StatWidgetClass);
		if (!PlayerStatWidget) {
			UE_LOG(LogTemp, Error, TEXT("void APRPlayerController::BeginPlay() : Not Binding HUDWidgetClass"));
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
	int32 Potion = PotionNum - 1;
	APawn* PlayerPawn = GetPawn();
	UStatComponent* PlayerStatComp = PlayerPawn ? Cast<ABaseCharacter>(PlayerPawn)->GetStatComponent() : nullptr;
	PlayerStatComp->ChangeStatCurrent(EFullStats::Health, 10);
	PlayerHUD->UpdatePotionNum(Potion);
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
// ========================================================
// 스탯 위젯 클래스
// ========================================================
void APRPlayerController::HandleStatInput()
{
	if (!StatWidgetClass) {
		UE_LOG(LogTemp, Warning, TEXT("StatWidgetClass is not set in PlayerController Blueprint."));
		return;
	}

	if (bIsStatWidgetOpen) { // 스탯창 닫는 로직
		PlayerStatWidget->RemoveFromParent();
		OnStatWindowClosed();
		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
		bShowMouseCursor = false;
		bIsStatWidgetOpen = false;
	}
	else {
		APawn* PlayerPawn = GetPawn();
		UStatComponent* PlayerStatComp = PlayerPawn ? Cast<ABaseCharacter>(PlayerPawn)->GetStatComponent() : nullptr;

		if (PlayerStatComp) {
			PlayerStatWidget->SetStatComponent(PlayerStatComp);
		}
		OnStatWindowOpened();
		PlayerStatWidget->AddToViewport();
		FInputModeGameAndUI InputMode;
		SetInputMode(InputMode);
		bShowMouseCursor = true;
		bIsStatWidgetOpen = true;
	}
}
void APRPlayerController::OnStatWindowOpened()
{
	APawn* PlayerPawn = GetPawn();
	APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(PlayerPawn);
	USceneCaptureComponent2D* CaptureComp = PlayerChar ? PlayerChar->GetPortraitCaptureComponent() : nullptr;

	if (CaptureComp) {
		CaptureComp->bCaptureEveryFrame = true;
		CaptureComp->CaptureScene();

		UE_LOG(LogTemp, Log, TEXT("Portrait Capture Started."));
	}
}
void APRPlayerController::OnStatWindowClosed()
{
	APawn* PlayerPawn = GetPawn();
	APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(PlayerPawn);
	USceneCaptureComponent2D* CaptureComp = PlayerChar ? PlayerChar->GetPortraitCaptureComponent() : nullptr;

	if (CaptureComp) {
		CaptureComp->bCaptureEveryFrame = false;
		UE_LOG(LogTemp, Log, TEXT("Portrait Capture Stopped."));
	}
}