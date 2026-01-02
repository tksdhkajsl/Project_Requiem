#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PREndingGameModeBase.generated.h"

class UEndingCreditWidget;

UCLASS()
class PROJECT_REQUIEM_API APREndingGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
#pragma region 발코니 걸어가는 씬
public:
	APREndingGameModeBase();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class AEndingPawn* EndingPawn = nullptr;

protected:
	virtual void BeginPlay() override;
#pragma endregion

#pragma region 걷는 씬 후 달 없어짐
	UFUNCTION()
	void RemovedMoon();

	UFUNCTION(BlueprintImplementableEvent)
	void FademoutMoon();

	FTimerHandle RemoveMoonTimerHandler;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MoonTimes = 5.f;
#pragma endregion

#pragma region 달 없어진 후 태양 떠오름
public:
	UFUNCTION()
	void SunRise();
	UFUNCTION(BlueprintImplementableEvent)
	void StartSunRiseSequence();

	FTimerHandle SunRiseTimerHandler;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sun")
	float SunRiseTime = 3.f; //
#pragma endregion

#pragma region 엔딩크레딧(버튼으로 메인메뉴 이동)
public:
	UFUNCTION()
	void ShowEndingCredits();
	UPROPERTY()
	TObjectPtr<UEndingCreditWidget> EndingCreditWidget;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UEndingCreditWidget> EndingCreditWidgetClass;
#pragma endregion

};