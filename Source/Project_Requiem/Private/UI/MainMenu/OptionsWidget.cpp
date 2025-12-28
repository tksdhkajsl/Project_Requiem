#include "UI/MainMenu/OptionsWidget.h"
#include "Components/Button.h"
#include "Components/Slider.h"
#include "Components/CheckBox.h"
#include "Components/ComboBoxString.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameUserSettings.h"
#include "Misc/CString.h" // FCString::Atof 사용을 위해 포함
#include "Sound/SoundClass.h"
#include "Components/Button.h"
#include "AudioDevice.h"

void UOptionsWidget::OnCloseClicked()
{
    OnOptionsClosed.Broadcast();
}

bool UOptionsWidget::Initialize()
{
    if (!Super::Initialize()) return false;

    if (CloseButton) CloseButton->OnClicked.AddDynamic(this, &UOptionsWidget::OnCloseClicked);

    // UGameUserSettings는 GEngine이 유효할 때 접근 가능
    UGameUserSettings* Settings = GEngine->GetGameUserSettings();

    // --- 볼륨 (Slider) ---
    if (VolumeSlider)
    {
        VolumeSlider->OnValueChanged.AddDynamic(this, &UOptionsWidget::OnVolumeChanged);
        VolumeSlider->SetValue(0.5f);
    }

    // --- VSync (CheckBox) ---
    if (VSyncCheckBox && Settings)
    {
        VSyncCheckBox->OnCheckStateChanged.AddDynamic(this, &UOptionsWidget::OnVSyncToggled);
        // 현재 VSync 상태를 불러와 초기 상태 설정
        VSyncCheckBox->SetIsChecked(Settings->IsVSyncEnabled());
    }

    // --- Display Resolution (ComboBox) ---
    if (ResolutionComboBox && Settings) {
        ResolutionComboBox->ClearOptions();
        ResolutionComboBox->AddOption(TEXT("1920x1080"));
        ResolutionComboBox->AddOption(TEXT("2560x1440"));
        ResolutionComboBox->AddOption(TEXT("3840x2160"));

        ResolutionComboBox->OnSelectionChanged.AddDynamic(this, &UOptionsWidget::OnResolutionChanged);

        // 현재 해상도를 불러와 초기값 설정
        FIntPoint CurrentRes = Settings->GetScreenResolution();
        FString CurrentResString = FString::Printf(TEXT("%dx%d"), CurrentRes.X, CurrentRes.Y);
        ResolutionComboBox->SetSelectedOption(CurrentResString);
    }

    // --- Window Mode (ComboBox) ---
    if (WindowModeComboBox && Settings)
    {
        WindowModeComboBox->ClearOptions();
        WindowModeComboBox->AddOption(TEXT("Windowed"));
        WindowModeComboBox->AddOption(TEXT("Fullscreen"));
        WindowModeComboBox->OnSelectionChanged.AddDynamic(this, &UOptionsWidget::OnWindowModeChanged);

        // 현재 창 모드를 불러와 초기값 설정
        EWindowMode::Type CurrentMode = Settings->GetFullscreenMode();
        FString CurrentModeString;
        if (CurrentMode == EWindowMode::Windowed) CurrentModeString = TEXT("Windowed");
        else if (CurrentMode == EWindowMode::Fullscreen) CurrentModeString = TEXT("Fullscreen");
        else CurrentModeString = TEXT("Windowed"); // 기타 모드 (예: WindowedFullscreen)는 Windowed로 기본 설정

        WindowModeComboBox->SetSelectedOption(CurrentModeString);
    }

    // --- Frame Rate (ComboBox) ---
    if (FrameRateComboBox && Settings)
    {
        FrameRateComboBox->ClearOptions();
        FrameRateComboBox->AddOption(TEXT("30"));
        FrameRateComboBox->AddOption(TEXT("60"));
        FrameRateComboBox->AddOption(TEXT("120"));
        FrameRateComboBox->OnSelectionChanged.AddDynamic(this, &UOptionsWidget::OnFrameRateChanged);

        // 현재 FPS 제한 값을 불러와 초기값 설정
        float CurrentFPS = Settings->GetFrameRateLimit();
        FString CurrentFPSString = FString::Printf(TEXT("%d"), FMath::RoundToInt(CurrentFPS));
        FrameRateComboBox->SetSelectedOption(CurrentFPSString);
    }

    // --- Graphics Quality (PostProcessing, AntiAliasing, Texture, Shadow) ---
    auto SetupQualityComboBox = [&](UComboBoxString* ComboBox, int32 CurrentLevel, void (UOptionsWidget::* Callback)(FString, ESelectInfo::Type))
        {
            if (ComboBox)
            {
                ComboBox->ClearOptions();
                ComboBox->AddOption(TEXT("Low"));
                ComboBox->AddOption(TEXT("Medium"));
                ComboBox->AddOption(TEXT("High"));
                ComboBox->AddOption(TEXT("Epic"));
                ComboBox->OnSelectionChanged.AddDynamic(this, Callback);

                // 현재 품질 레벨을 불러와 초기값 설정
                FString QualityString;
                switch (CurrentLevel)
                {
                case 0: QualityString = TEXT("Low"); break;
                case 1: QualityString = TEXT("Medium"); break;
                case 2: QualityString = TEXT("High"); break;
                case 3: QualityString = TEXT("Epic"); break;
                default: QualityString = TEXT("Medium"); break;
                }
                ComboBox->SetSelectedOption(QualityString);
            }
        };

    if (PostLow) PostLow->OnClicked.AddDynamic(this, &UOptionsWidget::OnPostLowClicked);
    if (PostMedium) PostMedium->OnClicked.AddDynamic(this, &UOptionsWidget::OnPostMediumClicked);
    if (PostHigh) PostHigh->OnClicked.AddDynamic(this, &UOptionsWidget::OnPostHighClicked);
    if (PostEpic) PostEpic->OnClicked.AddDynamic(this, &UOptionsWidget::OnPostEpicClicked);

    if (AALow) AALow->OnClicked.AddDynamic(this, &UOptionsWidget::OnAALowClicked);
    if (AAMedium) AAMedium->OnClicked.AddDynamic(this, &UOptionsWidget::OnAAMediumClicked);
    if (AAHigh) AAHigh->OnClicked.AddDynamic(this, &UOptionsWidget::OnAAHighClicked);
    if (AAEpic) AAEpic->OnClicked.AddDynamic(this, &UOptionsWidget::OnAAEpicClicked);

    if (TexLow) TexLow->OnClicked.AddDynamic(this, &UOptionsWidget::OnTexLowClicked);
    if (TexMedium) TexMedium->OnClicked.AddDynamic(this, &UOptionsWidget::OnTexMediumClicked);
    if (TexHigh) TexHigh->OnClicked.AddDynamic(this, &UOptionsWidget::OnTexHighClicked);
    if (TexEpic) TexEpic->OnClicked.AddDynamic(this, &UOptionsWidget::OnTexEpicClicked);

    if (ShadowLow) ShadowLow->OnClicked.AddDynamic(this, &UOptionsWidget::OnShadowLowClicked);
    if (ShadowMedium) ShadowMedium->OnClicked.AddDynamic(this, &UOptionsWidget::OnShadowMediumClicked);
    if (ShadowHigh) ShadowHigh->OnClicked.AddDynamic(this, &UOptionsWidget::OnShadowHighClicked);
    if (ShadowEpic) ShadowEpic->OnClicked.AddDynamic(this, &UOptionsWidget::OnShadowEpicClicked);

    return true;
}

// ===================================
//  콜백 함수 구현 (Callback Functions)
// ===================================

// --- 볼륨 (Slider) ---
void UOptionsWidget::OnVolumeChanged(float Value)
{
    if (MasterSoundMix)  // 위젯 블루프린트에서 할당
    {
        UGameplayStatics::SetSoundMixClassOverride(this, MasterSoundMix, MasterSoundClass, Value, 1.f, 0.f, true);
        UGameplayStatics::PushSoundMixModifier(this, MasterSoundMix);

        UE_LOG(LogTemp, Warning, TEXT("Master Volume set to %.2f"), Value);
    }
    PlayTestSound();

    //// MasterSoundMixClass 변수가 위젯 블루프린트에서 설정되어 있다면 사용
    //if (MasterSoundClass)
    //{
    //    MasterSoundClass->Properties.Volume = Value; // 0~1
    //}

    //FAudioDeviceHandle AudioDeviceHandle = FAudioDevice::GetMainAudioDevice();

    //// 실제 포인터로 접근
    //if (FAudioDevice* AudioDevice = AudioDeviceHandle.GetAudioDevice())
    //{
    //    // 마스터 볼륨 로그/확인
    //    UE_LOG(LogTemp, Warning, TEXT("Master Volume Slider Value: %.2f"), Value);

    //    // 볼륨 적용 (TransientPrimaryVolume은 내부적인 master 볼륨 계수)
    //    AudioDevice->TransientPrimaryVolume = Value;

    //    // 현재 오디오 디바이스 상의 볼륨 값 확인 가능
    //    float CurrentMaster = AudioDevice->GetMasterVolume();
    //    UE_LOG(LogTemp, Warning, TEXT("Current Audio Master Volume: %.2f"), CurrentMaster);
    //}
}

// --- VSync (CheckBox) ---
void UOptionsWidget::OnVSyncToggled(bool bIsChecked)
{
    if (UGameUserSettings* Settings = GEngine->GetGameUserSettings())
    {
        Settings->SetVSyncEnabled(bIsChecked);
        Settings->ApplySettings(false);
    }
}

// --- Window Mode (ComboBox) ---
void UOptionsWidget::OnWindowModeChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
    if (!GEngine) return;

    // --- 선택된 값으로 WindowMode 설정 ---
    EWindowMode::Type NewMode = EWindowMode::Windowed;
    if (SelectedItem == TEXT("Fullscreen"))
        NewMode = EWindowMode::Fullscreen;
    else if (SelectedItem == TEXT("WindowedFullscreen"))
        NewMode = EWindowMode::WindowedFullscreen;
    else if (SelectedItem == TEXT("Windowed"))
        NewMode = EWindowMode::Windowed;

    // --- 현재/선택된 해상도 구하기 ---
    int32 ResX = 0, ResY = 0;
    if (ResolutionComboBox)
    {
        FString ResStr = ResolutionComboBox->GetSelectedOption();
        FString XStr, YStr;
        if (ResStr.Split(TEXT("x"), &XStr, &YStr))
        {
            ResX = FCString::Atoi(*XStr);
            ResY = FCString::Atoi(*YStr);
        }
    }

    if (GEngine && GEngine->GameViewport)
    {
        FIntPoint NewRes(ResX, ResY);

        // ApplySettings로 내부값 업데이트
        if (UGameUserSettings* Settings = GEngine->GetGameUserSettings())
        {
            Settings->SetScreenResolution(NewRes);
            Settings->SetFullscreenMode(NewMode);
            Settings->ApplySettings(true);
        }

        //// GameViewport에서 직접 해상도/창 모드 적용
        //TSharedPtr<SWindow> Window = GEngine->GameViewport->GetWindow();
        //if (Window.IsValid())
        //{
        //    FVector2D Size(NewRes.X, NewRes.Y);
        //    EWindowMode::Type WindowMode = (NewMode == EWindowMode::Fullscreen) ? EWindowMode::Fullscreen : EWindowMode::Windowed;
        //    Window->Resize(Size);
        //    Window->SetWindowMode(WindowMode);
        //}
    }
}

// --- Display Resolution (ComboBox) ---
void UOptionsWidget::OnResolutionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
    if (UGameUserSettings* Settings = GEngine->GetGameUserSettings())
    {
        FIntPoint NewRes;

        if (SelectedItem == TEXT("1920x1080")) NewRes = FIntPoint(1920, 1080);
        else if (SelectedItem == TEXT("2560x1440")) NewRes = FIntPoint(2560, 1440);
        else if (SelectedItem == TEXT("3840x2160")) NewRes = FIntPoint(3840, 2160);
        else return; // 알 수 없는 해상도

        Settings->SetScreenResolution(NewRes);
        Settings->ApplySettings(false);
    }
}

// --- Frame Rate (ComboBox) ---
void UOptionsWidget::OnFrameRateChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
    if (UGameUserSettings* Settings = GEngine->GetGameUserSettings())
    {
        float FPS = FCString::Atof(*SelectedItem);
        Settings->SetFrameRateLimit(FPS);
        Settings->ApplySettings(false);

        GEngine->Exec(GetWorld(), *FString::Printf(TEXT("t.MaxFPS %d"), FMath::RoundToInt(FPS)));
    }
}

// --- 그래픽 품질 설정 공통 로직 ---
int32 GetQualityLevelFromSelection(const FString& SelectedItem)
{
    if (SelectedItem == TEXT("Low")) return 0;
    if (SelectedItem == TEXT("Medium")) return 1;
    if (SelectedItem == TEXT("High")) return 2;
    if (SelectedItem == TEXT("Epic")) return 3;
    return 1; // 기본값
}

// --- Graphics Quality (ComboBox) ---
void UOptionsWidget::UpdateButtonHighlight(UButton* Low, UButton* Medium, UButton* High, UButton* Epic, int32 SelectedLevel)
{
    if (!Low || !Medium || !High || !Epic) return;

    auto SetColor = [](UButton* Btn, bool bSelected)
        {
            Btn->SetBackgroundColor(bSelected ? FLinearColor::Yellow : FLinearColor::White);
        };

    SetColor(Low, SelectedLevel == 0);
    SetColor(Medium, SelectedLevel == 1);
    SetColor(High, SelectedLevel == 2);
    SetColor(Epic, SelectedLevel == 3);
}

void UOptionsWidget::SetPostProcessingQuality(int32 Level)
{
    if (UGameUserSettings* Settings = GEngine->GetGameUserSettings())
    {
        Settings->SetPostProcessingQuality(Level);
        Settings->ApplySettings(false);
    }
}

void UOptionsWidget::SetAntiAliasingQuality(int32 Level)
{
    if (UGameUserSettings* Settings = GEngine->GetGameUserSettings())
    {
        Settings->SetAntiAliasingQuality(Level);
        Settings->ApplySettings(false);
    }
}

void UOptionsWidget::SetTextureQuality(int32 Level)
{
    if (UGameUserSettings* Settings = GEngine->GetGameUserSettings())
    {
        Settings->SetTextureQuality(Level);
        Settings->ApplySettings(false);
    }
}

void UOptionsWidget::SetShadowQuality(int32 Level)
{
    if (UGameUserSettings* Settings = GEngine->GetGameUserSettings())
    {
        Settings->SetShadowQuality(Level);
        Settings->ApplySettings(false);
    }
}

void UOptionsWidget::PlayTestSound()
{
    if (TestSound)
    {
        UGameplayStatics::PlaySound2D(this, TestSound, 1.0f, 1.0f, 0.0f);
        UE_LOG(LogTemp, Warning, TEXT("Playing TestSound"));
    }
}

// --- PostProcessing ---
void UOptionsWidget::OnPostLowClicked() { SetPostProcessingQuality(0); UpdateButtonHighlight(PostLow, PostMedium, PostHigh, PostEpic, 0); }
void UOptionsWidget::OnPostMediumClicked() { SetPostProcessingQuality(1); UpdateButtonHighlight(PostLow, PostMedium, PostHigh, PostEpic, 1); }
void UOptionsWidget::OnPostHighClicked() { SetPostProcessingQuality(2); UpdateButtonHighlight(PostLow, PostMedium, PostHigh, PostEpic, 2); }
void UOptionsWidget::OnPostEpicClicked() { SetPostProcessingQuality(3); UpdateButtonHighlight(PostLow, PostMedium, PostHigh, PostEpic, 3); }

// --- AntiAliasing ---
void UOptionsWidget::OnAALowClicked() { SetAntiAliasingQuality(0); UpdateButtonHighlight(AALow, AAMedium, AAHigh, AAEpic, 0); }
void UOptionsWidget::OnAAMediumClicked() { SetAntiAliasingQuality(1); UpdateButtonHighlight(AALow, AAMedium, AAHigh, AAEpic, 1); }
void UOptionsWidget::OnAAHighClicked() { SetAntiAliasingQuality(2); UpdateButtonHighlight(AALow, AAMedium, AAHigh, AAEpic, 2); }
void UOptionsWidget::OnAAEpicClicked() { SetAntiAliasingQuality(3); UpdateButtonHighlight(AALow, AAMedium, AAHigh, AAEpic, 3); }

// --- Texture ---
void UOptionsWidget::OnTexLowClicked() { SetTextureQuality(0); UpdateButtonHighlight(TexLow, TexMedium, TexHigh, TexEpic, 0); }
void UOptionsWidget::OnTexMediumClicked() { SetTextureQuality(1); UpdateButtonHighlight(TexLow, TexMedium, TexHigh, TexEpic, 1); }
void UOptionsWidget::OnTexHighClicked() { SetTextureQuality(2); UpdateButtonHighlight(TexLow, TexMedium, TexHigh, TexEpic, 2); }
void UOptionsWidget::OnTexEpicClicked() { SetTextureQuality(3); UpdateButtonHighlight(TexLow, TexMedium, TexHigh, TexEpic, 3); }

// --- Shadow ---
void UOptionsWidget::OnShadowLowClicked() { SetShadowQuality(0); UpdateButtonHighlight(ShadowLow, ShadowMedium, ShadowHigh, ShadowEpic, 0); }
void UOptionsWidget::OnShadowMediumClicked() { SetShadowQuality(1); UpdateButtonHighlight(ShadowLow, ShadowMedium, ShadowHigh, ShadowEpic, 1); }
void UOptionsWidget::OnShadowHighClicked() { SetShadowQuality(2); UpdateButtonHighlight(ShadowLow, ShadowMedium, ShadowHigh, ShadowEpic, 2); }
void UOptionsWidget::OnShadowEpicClicked() { SetShadowQuality(3); UpdateButtonHighlight(ShadowLow, ShadowMedium, ShadowHigh, ShadowEpic, 3); }