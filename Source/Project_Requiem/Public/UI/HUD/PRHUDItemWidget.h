#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PRHUDItemWidget.generated.h"

class UTextBlock;
class UImage;

UCLASS()
class PROJECT_REQUIEM_API UPRHUDItemWidget : public UUserWidget
{
	GENERATED_BODY()
	
#pragma region 언리얼 기본 생성
protected:
	virtual void NativeConstruct() override;
#pragma endregion

#pragma region UI 변경용
public:
    UFUNCTION(BlueprintCallable, Category = "Item")
    void ChangeItemImage(UTexture2D* NewTexture);
    UFUNCTION(BlueprintCallable, Category = "Item")
    void ChangeItemNum(const FText& NewName);

private:
    /** 페이드 애니메이션 용 함수 */
    UFUNCTION()
    void FadeTick();

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> CurrImage;
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> NewImage;
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> ItemNum;

    // 페이드용 변수
    float FadeAlpha = 0.0f;
    TObjectPtr<UTexture2D> PendingTexture = nullptr;
#pragma endregion

};
