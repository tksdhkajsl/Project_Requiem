#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PRHUDBossWidget.generated.h"

class UTextBlock;
class UProgressBar;

UCLASS()
class PROJECT_REQUIEM_API UPRHUDBossWidget : public UUserWidget
{
	GENERATED_BODY()
	
#pragma region 언리얼 기본 생성
protected:
    virtual void NativeConstruct() override;
#pragma endregion

#pragma region UI 변경용
public:
    UFUNCTION(BlueprintCallable, Category = "Stat Bar")
    void SetStatValue(float CurrentValue, float MaxValue);
    UFUNCTION(BlueprintCallable, Category = "Stat Bar")
    void SetStatName(const FText& NewName);
    UFUNCTION(BlueprintCallable, Category = "Stat Bar")
    void SetBarColor(FLinearColor NewColor);

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> StatName;
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UProgressBar> StatValue;
#pragma endregion
};
