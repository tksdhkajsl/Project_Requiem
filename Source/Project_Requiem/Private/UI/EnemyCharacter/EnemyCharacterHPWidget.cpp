#include "UI/EnemyCharacter/EnemyCharacterHPWidget.h"
#include "Components/ProgressBar.h"

void UEnemyCharacterHPWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (HPBar_Current) HPBar_Current->SetPercent(1.f);
    if (HPBar_Delay) HPBar_Delay->SetPercent(1.f);
}

void UEnemyCharacterHPWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    // 딜레이 바가 현재 체력보다 높으면 천천히 줄어듦 (철권/소울류 효과)
    if (DelayPercent > TargetPercent)
    {
        float Step = StepPerSecond * InDeltaTime;
        DelayPercent = FMath::Max(DelayPercent - Step, TargetPercent);

        if (HPBar_Delay)
        {
            HPBar_Delay->SetPercent(DelayPercent);
        }
    }
}

void UEnemyCharacterHPWidget::UpdateHP(float Current, float Max)
{
    if (Max <= 0.f) return;

    TargetPercent = FMath::Clamp(Current / Max, 0.f, 1.f);

    if (HPBar_Current) HPBar_Current->SetPercent(TargetPercent);

    // 만약 리스폰 등으로 체력이 확 차오르면 딜레이 바도 즉시 채움
    if (TargetPercent > DelayPercent && HPBar_Delay)
    {
        DelayPercent = TargetPercent;
        HPBar_Delay->SetPercent(DelayPercent);
    }
}