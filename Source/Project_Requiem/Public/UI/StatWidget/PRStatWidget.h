#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PRStatWidget.generated.h"

class UPRPlayerPortraitWidget;
class UPRStatRenderWidget;
class UStatComponent;
<<<<<<< HEAD
class UTextBlock;
enum class ELevelUpStats : uint8;
enum class EFullStats : uint8;
enum class EWeaponCode : uint8;
enum class EWeaponRank : uint8;
=======
enum class EFullStats : uint8;
>>>>>>> 2bd40acd771b463067b062a5a771101532b23eb1

UCLASS()
class PROJECT_REQUIEM_API UPRStatWidget : public UUserWidget
{
	GENERATED_BODY()
	
#pragma region 언리얼 기본 생성
protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
<<<<<<< HEAD

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> SoulValueText;
    // =============================================================
    // 무기 랭크 텍스트 바인딩 (이름을 꼭 기억하세요!)
    // =============================================================
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> Text_OneHandedRank;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> Text_TwoHandedRank;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> Text_DualBladeRank;
    // =============================================================
=======
#pragma endregion
	
#pragma region 초상화 위젯
protected:
    ///**
    // * @brief 플레이어 3D 초상화(Scene Capture 2D 결과물)와 추가 정보가 표시되는 패널
    // * UMG 블루프린트에서 'UPRPlayerPortraitWidget'을 추가하고 변수 이름과 일치시켜야 합니다.
    // */
    //UPROPERTY(meta = (BindWidget))
    //TObjectPtr<UPRPlayerPortraitWidget> PortraitPanel;
>>>>>>> 2bd40acd771b463067b062a5a771101532b23eb1
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

<<<<<<< HEAD
#pragma region 무기 랭크 업데이트
public:
    // 외부에서 랭크가 바뀌면 호출할 함수
    void UpdateWeaponRank(EWeaponCode WeaponCode, EWeaponRank NewRank);

private:
    // 랭크 Enum을 FText("F", "S" 등)로 변환하는 헬퍼 함수
    FText GetRankText(EWeaponRank Rank);
#pragma endregion

=======
>>>>>>> 2bd40acd771b463067b062a5a771101532b23eb1
#pragma region 스탯컴포넌트 캐싱용
public:
    /**
     * @brief 이 위젯이 참조할 UStatComponent를 설정하고, 스탯 업데이트 델리게이트에 바인딩합니다.
     * @param Component 참조할 스탯 컴포넌트 포인터
     */
    UFUNCTION(BlueprintCallable, Category = "Stats")
    void SetStatComponent(UStatComponent* Component);

    void InitializeStatValues();
<<<<<<< HEAD

    UFUNCTION()
    void HandleBasicStatUpdate(ELevelUpStats StatType, int32 AllocatedPoints);
    UFUNCTION()
    void HandleRegenStatUpdate(EFullStats StatType, float CurrentValue, float MaxValue);
    UFUNCTION()
    void HandleExpChanged(float CurrentExp);
    UFUNCTION()
    void HandleRequestLevelUpStat(ELevelUpStats StatType);
=======
    UFUNCTION()
    void HandleRegenStatUpdate(EFullStats StatType, float CurrentValue, float MaxValue);
>>>>>>> 2bd40acd771b463067b062a5a771101532b23eb1

    FORCEINLINE UStatComponent* GetCachedStatComponent() const { return CachedStatComponent; }
private:
    UPROPERTY()
    TObjectPtr<UStatComponent> CachedStatComponent;
#pragma endregion
};
