#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Animation/WidgetAnimation.h"
#include "MainMenuWidget.generated.h"

class UButton;
class UOptionsWidget;

UCLASS()
class PROJECT_REQUIEM_API UMainMenuWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UOptionsWidget> OptionsWidget;

protected:
    UPROPERTY(meta = (BindWidget))
    class UVerticalBox* MenuBox;

    UPROPERTY(Transient, meta = (BindWidgetAnim))
    class UWidgetAnimation* MenuBoxAnimation;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> StartGameButton;
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> LoadGameButton; // (로드)
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> OptionsButton; // (옵션)
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> QuitGameButton; // (종료)

#pragma region 애니메이션 재생 함수
public:
    UFUNCTION(BlueprintCallable)
    void PlayMenuBoxAnimation();
#pragma endregion

#pragma region 버튼 구현 함수
    // 시작 버튼 클릭
    UFUNCTION()
    void OnStartGameClicked();
    // 로드 버튼 클릭 (함수만 선언하고 구현은 비워둡니다.)
    UFUNCTION()
    void OnLoadGameClicked();
    // 옵션 버튼 클릭
    UFUNCTION()
    void OnOptionsClicked();
    // 옵션창 종료
    UFUNCTION()
    void OnOptionsClosed();
    // 종료 버튼 클릭
    UFUNCTION()
    void OnQuitGameClicked();
#pragma endregion
};