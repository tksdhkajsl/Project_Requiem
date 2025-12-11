#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PRHUDPotionWidget.generated.h"

class UImage;
class UTextBlock;

UCLASS()
class PROJECT_REQUIEM_API UPRHUDPotionWidget : public UUserWidget
{
	GENERATED_BODY()
	
#pragma region 언리얼 기본 생성
public:
    UFUNCTION()
    void ItitItemImage(UTexture2D* NewTexture);
protected:
    virtual void NativeConstruct() override;
#pragma endregion

#pragma region UI 변경용
public:
    UFUNCTION(BlueprintCallable, Category = "Item")
    void ChangeItemNum(const FText& NewName);

private:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> ItemImage;
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> ItemNumText;
#pragma endregion
};
