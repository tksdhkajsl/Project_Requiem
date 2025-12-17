#include "UI/PlayerDeath/PlayerDeathWidget.h"
#include "Animation/WidgetAnimation.h"

// ========================================================
// 언리얼 기본 생성
// ========================================================
void UPlayerDeathWidget::NativeConstruct()
{
    if (!FadeInDeathAnim) UE_LOG(LogTemp, Error, TEXT("UPlayerDeathWidget::NativeConstruct() : FadeInDeathAnim Not Bind"));
}
// ========================================================
// 애니메이션
// ========================================================
void UPlayerDeathWidget::PlayDeathAnimation()
{
    if (FadeInDeathAnim) PlayAnimation(FadeInDeathAnim, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f);
}