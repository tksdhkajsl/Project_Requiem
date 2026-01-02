#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BonfireWidget.generated.h"

class UPRPlayerPortraitWidget;
class UPRStatRenderWidget;
class UStatComponent;
class UTextBlock;
enum class ELevelUpStats : uint8;
enum class EFullStats : uint8;

class UButton;
class APlayerCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBonfireClosed);

UCLASS()
class PROJECT_REQUIEM_API UBonfireWidget : public UUserWidget
{
	GENERATED_BODY()
	
#pragma region 델리게이트
public:
    UPROPERTY(BlueprintAssignable)
    FOnBonfireClosed OnBonfireClosed;
#pragma endregion


#pragma region 언리얼 기본 생성
protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> SoulValueText;
#pragma endregion
	
#pragma region 위젯 닫기 버튼
public:
    UFUNCTION()
    void CloseBonfire();
protected:
    // 포션 상점 버튼
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> CloseBonfireButton;
//private:
//    bool IsOpen = false;
#pragma endregion

#pragma region 포션 상점
public:
    // 구매 함수
    UFUNCTION()
    void OnBuyPotionClicked();
protected:
    // 포션 상점 버튼
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> BuyPotionButton;
    // 포션 가격
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
    int32 PotionPrice = 100;
    UPROPERTY()
    TObjectPtr<APlayerCharacter> PlayerRef;
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

    FORCEINLINE UStatComponent* GetCachedStatComponent() const { return CachedStatComponent; }
private:
    UPROPERTY()
    TObjectPtr<UStatComponent> CachedStatComponent;
#pragma endregion

public:
    // [추가] 12/31, 무기 숙련도 갱신
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> Onehand;  // 한손검 랭크 텍스트

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> Twohand; // 양손검 랭크 텍스트

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> Dual;  // 쌍검 랭크 텍스트

};
