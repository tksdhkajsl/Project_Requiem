#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TimelineComponent.h"
#include "EnemyCharacterHPWidget.generated.h"

class UProgressBar;
class UWidgetAnimation;

UCLASS()
class PROJECT_REQUIEM_API UEnemyCharacterHPWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Current / Max HP 갱신
	void UpdateHP(float Current, float Max);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HPBar_Current;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HPBar_Delay;

private:
	float TargetPercent = 1.f;    // 현재 HP
	float DelayPercent = 1.f;     // Delay HP
	float StepPerSecond = 0.03f;   // 초당 감소 비율 (0~1)
};