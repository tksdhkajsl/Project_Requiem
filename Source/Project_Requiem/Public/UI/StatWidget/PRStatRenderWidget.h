#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Stats/Data/EFullStats.h"
#include "PRStatRenderWidget.generated.h"

class UTextBlock;

UCLASS()
class PROJECT_REQUIEM_API UPRStatRenderWidget : public UUserWidget
{
	GENERATED_BODY()
	
#pragma region 언리얼 기본 생성
protected:
	virtual void NativeConstruct() override;
#pragma endregion



#pragma region 스탯 갱신
public:
    /**
     * @brief Health, Stamina와 같이 현재 값과 최대 값이 있는 스탯을 업데이트합니다.
     * @param StatType 업데이트할 EFullStats 타입 (Health, Stamina)
     * @param CurrentValue 현재 값
     * @param MaxValue 최대 값
     */
    UFUNCTION(BlueprintCallable, Category = "Stats")
    void UpdateResourceStat(EFullStats StatType, float CurrentValue, float MaxValue);
    /**
     * @brief PhysicalAttack/Defense와 같이 단일 값이 있는 스탯을 업데이트합니다.
     * @param StatType 업데이트할 EFullStats 타입 (Attack, Defense)
     * @param Value 스탯의 최종 값
     */
    UFUNCTION(BlueprintCallable, Category = "Stats")
    void UpdateSingleStat(EFullStats StatType, float Value);

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> HealthValueText;
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> MaxHealthValueText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> StaminaValueText;
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> MaxStaminaValueText;

    // 2. 단일 계산 스탯 (공격/방어) - 값 표시 텍스트
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> PhysicalAttackText;
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> MagicAttackText;
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> PhysicalDefenseText;
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> MagicDefenseText;
#pragma endregion

};