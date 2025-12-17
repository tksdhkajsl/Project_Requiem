#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerDeathWidget.generated.h"


UCLASS()
class PROJECT_REQUIEM_API UPlayerDeathWidget : public UUserWidget
{
	GENERATED_BODY()
	
#pragma region 언리얼 기본 생성
protected:
    virtual void NativeConstruct() override;
#pragma endregion

#pragma region 애니메이션
public:
    void PlayDeathAnimation();
    UPROPERTY(meta = (BindWidgetAnim), Transient)
    UWidgetAnimation* FadeInDeathAnim;
#pragma endregion
};
