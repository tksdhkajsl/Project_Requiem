#include "Core/PRGameModeBase.h"
#include "Core/PRPlayerController.h"
// ========================================================
// 언리얼 기본생성
// ========================================================
APRGameModeBase::APRGameModeBase()
{
	PlayerControllerClass = APRPlayerController::StaticClass();
}
void APRGameModeBase::BeginPlay()
{
	Super::BeginPlay();
}
