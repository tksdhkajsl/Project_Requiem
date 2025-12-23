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

//public:
//	void UpdateHP(float Current, float Max);
//
//protected:
//	// 상위 클래스 호출 및 초기화는 반드시 필요합니다.
//	virtual void NativeConstruct() override;
//
//	// 틱은 딜레이 바의 부드러운 움직임(FInterpTo)을 처리합니다.
//	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
//
//private:
//	// 타이머 만료 시 호출될 함수입니다. (움직임 시작)
//	UFUNCTION()
//	void StartDelayMovement();
//
//protected:
//	UPROPERTY(meta = (BindWidget))
//	TObjectPtr<UProgressBar> HPBar_Current; // 즉시 감소하는 HP 바
//
//	UPROPERTY(meta = (BindWidget))
//	TObjectPtr<UProgressBar> HPBar_Delay; // 천천히 뒤따르는 딜레이 바
//
//private:
//	// 현재 HP 목표 값 (0.0 ~ 1.0)
//	float TargetPercent = 1.f;
//	// 딜레이 바의 현재 값 (NativeTick에서 FInterpTo로 TargetPercent를 따라감)
//	float DelayPercent = 1.f;
//
//	// 딜레이 바가 목표를 따라잡는 속도 (낮을수록 느림, 철권 스타일은 0.5f ~ 2.0f 권장)
//	float InterpSpeed = 0.05f;
//
//	// ⭐ HP 감소 후, 딜레이 바 움직임이 시작되기까지 대기하는 시간 (철권 스타일 딜레이)
//	const float DelayStartTime = 1.0f; // 예: 1.0초 동안 멈춤
//
//	// 딜레이 타이머 관리를 위한 핸들
//	FTimerHandle DelayTimerHandle;
//
//	bool bIsDelayingMovement = false;

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