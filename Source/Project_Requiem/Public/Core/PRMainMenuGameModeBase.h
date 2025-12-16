#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PRMainMenuGameModeBase.generated.h"

class UMainMenuWidget;

UCLASS()
class PROJECT_REQUIEM_API APRMainMenuGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
#pragma region 언리얼 기본생성
public:
	APRMainMenuGameModeBase();
protected:
	virtual void BeginPlay() override;
#pragma endregion


#pragma region 시네마틱 카메라 움직임 멈췄을때(끝났을 때), 실행될 함수
public:
	UPROPERTY()
	TObjectPtr<UMainMenuWidget> MainMenuWidget;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UMainMenuWidget> MainMenuWidgetClass;

	UFUNCTION()
	void ShowMainMenu();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class APRMenuPawn* MenuPawn;
#pragma endregion

};
