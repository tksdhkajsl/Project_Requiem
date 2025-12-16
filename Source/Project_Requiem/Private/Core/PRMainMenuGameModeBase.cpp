#include "Core/PRMainMenuGameModeBase.h"
#include "UI/MainMenu/MainMenuWidget.h"
#include "UI/MainMenu/PRMenuPawn.h"
#include <Kismet/GameplayStatics.h>
#include <GameFramework/PlayerController.h>

// ========================================================
// 언리얼 기본생성
// ========================================================
APRMainMenuGameModeBase::APRMainMenuGameModeBase()
{

}
void APRMainMenuGameModeBase::BeginPlay()
{
	Super::BeginPlay();


    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

    TArray<AActor*> FoundPawns;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APRMenuPawn::StaticClass(), FoundPawns);

    if (FoundPawns.Num() > 0) {
        MenuPawn = Cast<APRMenuPawn>(FoundPawns[0]);

        if (MenuPawn && PlayerController) {
            PlayerController->Possess(MenuPawn);
            MenuPawn->OnCinematicFinished.AddDynamic(this, &APRMainMenuGameModeBase::ShowMainMenu);
        }
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
