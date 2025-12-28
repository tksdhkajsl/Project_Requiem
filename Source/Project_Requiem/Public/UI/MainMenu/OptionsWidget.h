#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OptionsWidget.generated.h"

class USlider;
class UCheckBox;
class UButton;
class UTextBlock;
class UComboBoxString;

DECLARE_MULTICAST_DELEGATE(FOnOptionsClosed);

UCLASS()
class PROJECT_REQUIEM_API UOptionsWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    FOnOptionsClosed OnOptionsClosed;
    UFUNCTION()
    void OnCloseClicked();
public:
    virtual bool Initialize() override;

protected:
    // ===================================
    //  UI 위젯 변수 (BindWidget)
    // ===================================

    // --- 닫기 버튼 ---
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> CloseButton;

    // --- 볼륨 ---
    UPROPERTY(meta = (BindWidget))
    USlider* VolumeSlider;

    // --- VSync ---
    UPROPERTY(meta = (BindWidget))
    UCheckBox* VSyncCheckBox;

    // --- Window Mode ---
    UPROPERTY(meta = (BindWidget))
    UComboBoxString* WindowModeComboBox;

    // --- Display Resolution ---
    UPROPERTY(meta = (BindWidget))
    UComboBoxString* ResolutionComboBox;

    // --- Frame Rate ---
    UPROPERTY(meta = (BindWidget))
    UComboBoxString* FrameRateComboBox;

    // --- Graphics Quality Buttons ---
    UPROPERTY(meta = (BindWidget)) UButton* PostLow;
    UPROPERTY(meta = (BindWidget)) UButton* PostMedium;
    UPROPERTY(meta = (BindWidget)) UButton* PostHigh;
    UPROPERTY(meta = (BindWidget)) UButton* PostEpic;

    UPROPERTY(meta = (BindWidget)) UButton* AALow;
    UPROPERTY(meta = (BindWidget)) UButton* AAMedium;
    UPROPERTY(meta = (BindWidget)) UButton* AAHigh;
    UPROPERTY(meta = (BindWidget)) UButton* AAEpic;

    UPROPERTY(meta = (BindWidget)) UButton* TexLow;
    UPROPERTY(meta = (BindWidget)) UButton* TexMedium;
    UPROPERTY(meta = (BindWidget)) UButton* TexHigh;
    UPROPERTY(meta = (BindWidget)) UButton* TexEpic;

    UPROPERTY(meta = (BindWidget)) UButton* ShadowLow;
    UPROPERTY(meta = (BindWidget)) UButton* ShadowMedium;
    UPROPERTY(meta = (BindWidget)) UButton* ShadowHigh;
    UPROPERTY(meta = (BindWidget)) UButton* ShadowEpic;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    USoundClass* MasterSoundClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    class USoundMix* MasterSoundMix;

private:
    // ===================================
    //  Callback Functions
    // ===================================

    // --- 볼륨 / VSync ---
    UFUNCTION()
    void OnVolumeChanged(float Value);

    UFUNCTION()
    void OnVSyncToggled(bool bIsChecked);

    // --- 화면 설정 (콤보 박스) ---
    UFUNCTION()
    void OnWindowModeChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

    UFUNCTION()
    void OnResolutionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

    UFUNCTION()
    void OnFrameRateChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

    // --- 그래픽 품질 (콤보 박스) ---
    //UFUNCTION() void OnVolumeChanged(float Value);
    //UFUNCTION() void OnVSyncToggled(bool bIsChecked);
    //UFUNCTION() void OnWindowModeChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
    //UFUNCTION() void OnFrameRateChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

    // Graphics Quality
    UFUNCTION() void OnPostLowClicked();
    UFUNCTION() void OnPostMediumClicked();
    UFUNCTION() void OnPostHighClicked();
    UFUNCTION() void OnPostEpicClicked();

    UFUNCTION() void OnAALowClicked();
    UFUNCTION() void OnAAMediumClicked();
    UFUNCTION() void OnAAHighClicked();
    UFUNCTION() void OnAAEpicClicked();

    UFUNCTION() void OnTexLowClicked();
    UFUNCTION() void OnTexMediumClicked();
    UFUNCTION() void OnTexHighClicked();
    UFUNCTION() void OnTexEpicClicked();

    UFUNCTION() void OnShadowLowClicked();
    UFUNCTION() void OnShadowMediumClicked();
    UFUNCTION() void OnShadowHighClicked();
    UFUNCTION() void OnShadowEpicClicked();

    // --- 버튼 강조 표시 ---
    void UpdateButtonHighlight(UButton* Low, UButton* Medium, UButton* High, UButton* Epic, int32 SelectedLevel);

    // Graphics Quality
    UFUNCTION()
    void SetPostProcessingQuality(int32 Level);
    UFUNCTION()
    void SetAntiAliasingQuality(int32 Level);
    UFUNCTION()
    void SetTextureQuality(int32 Level);
    UFUNCTION()
    void SetShadowQuality(int32 Level);

    // 디버그용
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    USoundWave* TestSound;
    void PlayTestSound();
};