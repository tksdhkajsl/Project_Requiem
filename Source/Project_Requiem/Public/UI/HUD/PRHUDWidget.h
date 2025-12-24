#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Stats/Data/EFullStats.h"
#include "PRHUDWidget.generated.h"

class UPRHUDStatBarWidget;
class UPRHUDBossWidget;
class UPRHUDItemWidget;
class UPRHUDPotionWidget;
class UTexture2D;
class UTextBlock;
class APlayerCharacter;

UCLASS()
class PROJECT_REQUIEM_API UPRHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
#pragma region 언리얼 기본 생성 및 초기화
protected:
    virtual void NativeConstruct() override;
    UFUNCTION()
    void BindToPawn(APawn* NewPawn);
    UFUNCTION()
    void OnPawnChanged(APawn* NewPawn);
#pragma endregion

#pragma region 스탯 바 갱신
public:
    // 델리게이트용 스탯바 업데이트 함수
    UFUNCTION()
    void HandleRegenStatChanged(EFullStats StatType, float CurrValue, float MaxValue);
    // HP바 업데이트하는 함수
    UFUNCTION(BlueprintCallable, Category = "HUD Control")
    void UpdateHPBar(float CurrentValue, float MaxValue);
    // Stamina바 업데이트하는 함수
    UFUNCTION(BlueprintCallable, Category = "HUD Control")
    void UpdateSTBar(float CurrentValue, float MaxValue);

    // BarHP (HP 상태 바)
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UPRHUDStatBarWidget> BarHP;
    // BarST (ST 상태 바)
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UPRHUDStatBarWidget> BarST;
#pragma endregion

#pragma region 보스 HP바 갱신
public:
    UFUNCTION()
    void ShowBossHPBar(bool bShow);
    UFUNCTION(BlueprintCallable, Category = "Boss")
    void SetBossName(const FText& Name);
    // 델리게이트용 스탯바 업데이트 함수
    UFUNCTION()
    void HandleBossStatChanged(EFullStats StatType, float CurrValue, float MaxValue);
    // Boss HP바 업데이트하는 함수
    UFUNCTION(Category = "HUD Control")
    void UpdateBossHPBar(float CurrentValue, float MaxValue);

    // BarHP (HP 상태 바)
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UPRHUDBossWidget> BossBarHP;
#pragma endregion

#pragma region 무기 아이템 스왑용
public:
    // 웨펀 스왑용 업데이트 함수
    UFUNCTION(BlueprintCallable)
    void UpdateWeaponSlot(int32 SlotIndex);

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UPRHUDItemWidget> WeaponSlot;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Slot")
    UTexture2D* ImageSlot0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Slot")
    UTexture2D* ImageSlot1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Slot")
    UTexture2D* ImageSlot2;
#pragma endregion

#pragma region 포션용 위젯
public:
    // 포션 업데이트 함수
    UFUNCTION(BlueprintCallable)
    void UpdatePotionNum(int32 ItemNum);

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UPRHUDPotionWidget> PotionSlot;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Potion Slot")
    UTexture2D* PotionImage;
#pragma endregion

#pragma region 인터렉션
public:
    UFUNCTION(BlueprintCallable)
    void UpdateActionText(const FText& text);

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UTextBlock* ActionTextValue;
#pragma endregion

#pragma region 캐싱용
    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<APlayerCharacter> Player;
#pragma endregion
};
