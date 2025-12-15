#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PRGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_REQUIEM_API APRGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
#pragma region 언리얼 기본생성
public:
	APRGameModeBase();
protected:
	virtual void BeginPlay() override;
#pragma endregion
};
