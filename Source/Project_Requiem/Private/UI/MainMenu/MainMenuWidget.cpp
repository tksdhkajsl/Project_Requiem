#include "UI/MainMenu/MainMenuWidget.h"
#include "UI/MainMenu/OptionsWidget.h"

#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "Kismet/KismetMathLibrary.h"
#include "Animation/UMGSequencePlayer.h"

// ========================================================
// 언리얼 기본 생성
// ========================================================
void UMainMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (OptionsWidget) {
        OptionsWidget->SetVisibility(ESlateVisibility::Collapsed);
        OptionsWidget->OnOptionsClosed.AddUObject(this, &UMainMenuWidget::OnOptionsClosed);
    }

    if (StartGameButton) {
        StartGameButton->OnClicked.RemoveAll(this);
        StartGameButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnStartGameClicked);
    }
    if (LoadGameButton) {
        LoadGameButton->OnClicked.RemoveAll(this);
        LoadGameButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnLoadGameClicked);
    }
    if (OptionsButton) {
        OptionsButton->OnClicked.RemoveAll(this);
        OptionsButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnOptionsClicked);
    }
    if (QuitGameButton) {
        QuitGameButton->OnClicked.RemoveAll(this);
        QuitGameButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnQuitGameClicked);
    }

    if (MenuBoxAnimation) {
        PlayMenuBoxAnimation();
    }
}
// ========================================================
// 애니메이션 재생 함수
// ========================================================
void UMainMenuWidget::PlayMenuBoxAnimation()
{
    if (MenuBoxAnimation)
    {
        PlayAnimation(MenuBoxAnimation, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f, false);
    }
}
// ========================================================
// 버튼 구현 함수
// ========================================================
void UMainMenuWidget::OnStartGameClicked()
{
}
void UMainMenuWidget::OnLoadGameClicked()
{
}
void UMainMenuWidget::OnOptionsClicked()
{
    if (!OptionsWidget) return;

    OptionsWidget->SetVisibility(ESlateVisibility::Visible);
}
void UMainMenuWidget::OnOptionsClosed()
{
    if (!OptionsWidget) return;

    OptionsWidget->SetVisibility(ESlateVisibility::Collapsed);
}
void UMainMenuWidget::OnQuitGameClicked()
{
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (PC) PC->ConsoleCommand("quit");
}
