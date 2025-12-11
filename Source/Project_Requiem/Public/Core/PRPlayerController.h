#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PRPlayerController.generated.h"

class UPRHUDWidget;

UCLASS()
class PROJECT_REQUIEM_API APRPlayerController : public APlayerController
{
	GENERATED_BODY()
	
	
#pragma region 언리얼 기본 생성
public:
	APRPlayerController();
protected:
	virtual void BeginPlay() override;
#pragma endregion

	/// Todo : 플레이어 할 행동
	/*
		if (APRPlayerController* PC = GetController<APRPlayerController>()) {
		PC->PushDownKeyboard1~3();	 // 키 바인딩해야함
		// 또는
		if (APRPlayerController* PC = GetController<APRPlayerController>()) {
		PC->PushDownKeyboard1~4(int32 포션갯수);	 // 키 바인딩해야함
	*/
#pragma region HUD 위젯 클래스
public:
	// HUD 위젯 클래스 (언리얼 에디터에서 Blueprint HUD 클래스를 지정하기 위해 사용)
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UPRHUDWidget> HUDWidgetClass;
	// 현재 생성된 HUD 위젯 인스턴스
	UPROPERTY()
	TObjectPtr<UPRHUDWidget> PlayerHUD;

	/** 1번째 무기 바인딩 */
	UFUNCTION()
	void PushDownKeyboard1();
	/** 2번째 무기 바인딩 */
	UFUNCTION()
	void PushDownKeyboard2();
	/** 3번째 무기 바인딩 */
	UFUNCTION()
	void PushDownKeyboard3();
	/** 포션 용 키 바인딩 */
	UFUNCTION()
	void PushDownKeyboard4(int32 PotionNum);
#pragma endregion

};
