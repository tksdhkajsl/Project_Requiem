#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PRPlayerController.generated.h"

class UPRHUDWidget;
class UPRStatWidget;
class ABaseCharacter;
class USceneCaptureComponent2D;

UCLASS()
class PROJECT_REQUIEM_API APRPlayerController : public APlayerController
{
	GENERATED_BODY()
	
	
#pragma region 언리얼 기본 생성
public:
	APRPlayerController();
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
#pragma endregion

#pragma region HUD 위젯 클래스
public:
	// HUD 위젯 클래스 (언리얼 에디터에서 Blueprint HUD 클래스를 지정하기 위해 사용)
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UPRHUDWidget> HUDWidgetClass;
	// 현재 생성된 HUD 위젯 인스턴스
	UPROPERTY()
	TObjectPtr<UPRHUDWidget> PlayerHUD;

	/** 1번째 무기 바인딩 */
	UFUNCTION(BlueprintCallable)
	void PushDownKeyboard1();
	/** 2번째 무기 바인딩 */
	UFUNCTION(BlueprintCallable)
	void PushDownKeyboard2();
	/** 3번째 무기 바인딩 */
	UFUNCTION(BlueprintCallable)
	void PushDownKeyboard3();
	/** 포션 용 키 바인딩 */
	UFUNCTION(BlueprintCallable)
	void PushDownKeyboard4(int32 PotionNum);
#pragma endregion

#pragma region 보스 관련 HUD 갱신
public:
	UFUNCTION()
	void OnEnterBossRoom(ABaseCharacter* Boss);
	UFUNCTION()
	void HandleBossHPChanged(EFullStats StatType, float Curr, float Max);

	UPROPERTY(VisibleAnywhere, Transient, Category = "Boss Data")
	ABaseCharacter* ActiveBoss = nullptr;
#pragma endregion

#pragma region 스탯 위젯 클래스
public:
	/** 스탯 위젯 여는 함수 */
	UFUNCTION(BlueprintCallable)
	void HandleStatInput();

	// Stat 위젯 클래스 (언리얼 에디터에서 Blueprint Stat 클래스를 지정하기 위해 사용)
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UPRStatWidget> StatWidgetClass;
	// 현재 생성된 Stat 위젯 인스턴스
	UPROPERTY()
	TObjectPtr<UPRStatWidget> PlayerStatWidget;

	/** @brief 스탯 창이 열릴 때 호출 (캡처 시작) */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void OnStatWindowOpened();

	/** @brief 스탯 창이 닫힐 때 호출 (캡처 중지) */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void OnStatWindowClosed();

private:
	// 스탯 위젯이 열렸는지 닫혔는지 상태를 추적하는 변수
	bool bIsStatWidgetOpen = false;
#pragma endregion

};
