#include "Core/PRMainMenuGameModeBase.h"
#include "UI/MainMenu/MainMenuWidget.h"
#include "UI/MainMenu/PRMenuPawn.h"
#include <Kismet/GameplayStatics.h>
#include <GameFramework/PlayerController.h>
#include "Core/PRGameInstance.h"

// ========================================================
// 언리얼 기본생성
// ========================================================
APRMainMenuGameModeBase::APRMainMenuGameModeBase()
{

}
/// 코드 수정 : 12/25 기존 두번 반복하던 것을 한번만 하도록 수행
void APRMainMenuGameModeBase::BeginPlay()
{
    Super::BeginPlay();

    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PlayerController) return;

    TArray<AActor*> FoundPawns;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APRMenuPawn::StaticClass(), FoundPawns);
    if (FoundPawns.Num() == 0) return;

    MenuPawn = Cast<APRMenuPawn>(FoundPawns[0]);
    if (!MenuPawn) return;

    PlayerController->Possess(MenuPawn);

    // Delegate는 안전하게 한 번만
    MenuPawn->OnCinematicFinished.RemoveDynamic(this, &APRMainMenuGameModeBase::ShowMainMenu);
    MenuPawn->OnCinematicFinished.AddDynamic(this, &APRMainMenuGameModeBase::ShowMainMenu);

    if (UPRGameInstance* GI = Cast<UPRGameInstance>(GetGameInstance())) {
        if (!GI->bHasPlayedIntroVideo) {
            GI->bHasPlayedIntroVideo = true;
            MenuPawn->StartCinematic();
        }
        else ShowMainMenu();
    }
}

void APRMainMenuGameModeBase::ShowMainMenu()
{
    if (MainMenuWidgetClass) {
        APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
        MainMenuWidget = CreateWidget<UMainMenuWidget>(PC, MainMenuWidgetClass);
        if (MainMenuWidget) {
            MainMenuWidget->AddToViewport(100);
            FInputModeUIOnly InputMode;
            PC->SetInputMode(InputMode);
            PC->SetShowMouseCursor(true);
        }
    }
}
