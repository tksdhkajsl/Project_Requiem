#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerDeathWidget.generated.h"

class UButton;

UCLASS()
class PROJECT_REQUIEM_API UPlayerDeathWidget : public UUserWidget
{
	GENERATED_BODY()
	
#pragma region 언리얼 기본 생성
protected:
    virtual void NativeConstruct() override;
#pragma endregion
	
#pragma region 애니메이션
public:
    void PlayDeathAnimation();
    UPROPERTY(meta = (BindWidgetAnim), Transient)
    UWidgetAnimation* FadeInDeathAnim;
#pragma endregion

#pragma region 버튼
public:
    UFUNCTION()
    void OnRetryClicked();
    UFUNCTION()
    void OnMainMenuClicked();

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> RetryButton;
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> MainMenuButton;
#pragma endregion
};
