#include "Core/PREndingGameModeBase.h"
#include "UI/Ending/EndingPawn.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Ending/EndingCreditWidget.h"
#include "Core/PRGameInstance.h"

APREndingGameModeBase::APREndingGameModeBase()
{

}
void APREndingGameModeBase::BeginPlay()
{
    Super::BeginPlay();

    UPRGameInstance* GI = Cast<UPRGameInstance>(GetGameInstance());
    if (!GI) return;

    GI->SavedWeaponMastery = nullptr;
    GI->SavedStatComponent = nullptr;

    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

    TArray<AActor*> FoundPawns;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEndingPawn::StaticClass(), FoundPawns);

    if (FoundPawns.Num() > 0 && PlayerController) {
        EndingPawn = Cast<AEndingPawn>(FoundPawns[0]);
        if (EndingPawn) {
            PlayerController->Possess(EndingPawn);
            EndingPawn->OnEndingWalkingFinished.AddDynamic(this, &APREndingGameModeBase::RemovedMoon);
            EndingPawn->EndingWalkingCinematic();
        }
    }
}
void APREndingGameModeBase::RemovedMoon()
{
    FademoutMoon();
    GetWorldTimerManager().SetTimer(RemoveMoonTimerHandler, this, &APREndingGameModeBase::SunRise, MoonTimes, false);
}
void APREndingGameModeBase::SunRise()
{
    StartSunRiseSequence();
    GetWorldTimerManager().SetTimer(SunRiseTimerHandler, this, &APREndingGameModeBase::ShowEndingCredits, SunRiseTime, false);
    EndingPawn->StartLookAtSun();
}
void APREndingGameModeBase::ShowEndingCredits()
{
    if (EndingCreditWidget) return;

    if (EndingCreditWidgetClass) {
        EndingCreditWidget = CreateWidget<UEndingCreditWidget>(GetWorld(), EndingCreditWidgetClass);
        if (EndingCreditWidget) {
            EndingCreditWidget->AddToViewport(10);
            EndingCreditWidget->StartCredits();

            APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
            if (PC) {
                FInputModeUIOnly InputMode;
                InputMode.SetWidgetToFocus(EndingCreditWidget->TakeWidget());
                PC->SetInputMode(InputMode);
                PC->bShowMouseCursor = true;
            }
        }
    }
}