#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PRStatWidget.generated.h"

class UPRPlayerPortraitWidget;
class UPRStatRenderWidget;
class UStatComponent;
class UWeaponMasteryComponent;
class UTextBlock;
enum class ELevelUpStats : uint8;
enum class EFullStats : uint8;

UCLASS()
class PROJECT_REQUIEM_API UPRStatWidget : public UUserWidget
{
	GENERATED_BODY()
	
#pragma region 언리얼 기본 생성
protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> SoulValueText;
#pragma endregion

#pragma region 스탯 렌더 위젯
public:
    /**
     * @brief 서브 위젯 인스턴스를 반환하여 외부에서 스탯 업데이트 접근을 용이하게 합니다.
     * @return 스탯 렌더링 위젯 포인터
     */
    UFUNCTION(BlueprintPure, Category = "Stats")
    UPRStatRenderWidget* GetStatRenderListWidget() const { return StatRenderList; }
protected:
    /**
     * @brief 스탯 목록(Health, Stamina, Attack, Defense 등)을 표시하는 위젯
     * UMG 블루프린트에서 'UPRStatRenderWidget'을 추가하고 변수 이름과 일치시켜야 합니다.
     */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UPRStatRenderWidget> StatRenderList;
#pragma endregion

#pragma region 스탯컴포넌트 캐싱용
public:
    /**
     * @brief 이 위젯이 참조할 UStatComponent를 설정하고, 스탯 업데이트 델리게이트에 바인딩합니다.
     * @param Component 참조할 스탯 컴포넌트 포인터
     */
    UFUNCTION(BlueprintCallable, Category = "Stats")
    void SetStatComponent(UStatComponent* Component);

    void InitializeStatValues();

    UFUNCTION()
    void HandleBasicStatUpdate(ELevelUpStats StatType, int32 AllocatedPoints);
    UFUNCTION()
    void HandleRegenStatUpdate(EFullStats StatType, float CurrentValue, float MaxValue);
    UFUNCTION()
    void HandleExpChanged(float CurrentExp);
    UFUNCTION()
    void HandleRequestLevelUpStat(ELevelUpStats StatType);

private:
    UPROPERTY()
    TObjectPtr<UStatComponent> CachedStatComponent;
#pragma endregion

#pragma region 웨폰마스터리
public:
    UFUNCTION(BlueprintCallable, Category = "WeaponMastery")
    void SetWeaponMasteryComponent(UWeaponMasteryComponent* Component);

    FORCEINLINE UStatComponent* GetCachedStatComponent() const { return CachedStatComponent; }
private:
    UPROPERTY()
    TObjectPtr<UWeaponMasteryComponent> CachedWeaponMastery;

    
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> TextOnehand;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> TextTwohand;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> TextDual;
#pragma endregion

};
