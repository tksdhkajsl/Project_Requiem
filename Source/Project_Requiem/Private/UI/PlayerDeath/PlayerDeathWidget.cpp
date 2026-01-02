#include "UI/PlayerDeath/PlayerDeathWidget.h"
#include "Animation/WidgetAnimation.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/Player/Character/PlayerCharacter.h"

// ========================================================
// 언리얼 기본 생성
// ========================================================
void UPlayerDeathWidget::NativeConstruct()
{
    if (!FadeInDeathAnim) UE_LOG(LogTemp, Error, TEXT("UPlayerDeathWidget::NativeConstruct() : FadeInDeathAnim Not Bind"));

    if (RetryButton) RetryButton->OnClicked.AddDynamic(this, &UPlayerDeathWidget::OnRetryClicked);
    if (MainMenuButton) MainMenuButton->OnClicked.AddDynamic(this, &UPlayerDeathWidget::OnMainMenuClicked);
}
// ========================================================
// 애니메이션
// ========================================================
void UPlayerDeathWidget::PlayDeathAnimation()
{
    if (FadeInDeathAnim) PlayAnimation(FadeInDeathAnim, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f);
}
// ========================================================
// 버튼
// ========================================================
void UPlayerDeathWidget::OnRetryClicked()
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PC) return;

    APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(PC->GetPawn());
    if (PlayerChar) PlayerChar->RespawnPlayer();
}

void UPlayerDeathWidget::OnMainMenuClicked()
{
    UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("MainMenuLevel")));
}