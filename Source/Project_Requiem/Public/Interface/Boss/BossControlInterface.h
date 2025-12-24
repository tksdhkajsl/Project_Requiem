#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BossControlInterface.generated.h"

UINTERFACE(MinimalAPI)
class UBossControlInterface : public UInterface 
{
    GENERATED_BODY()
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnBossStatUpdated, float, CurHP, float, MaxHP, FText, Name);
class PROJECT_REQUIEM_API IBossControlInterface
{
    GENERATED_BODY()

public:
    // 문이 열릴 때 보스에게 내릴 명령
    virtual void ActivateBossBattle() = 0;

    // 보스에게 플레이어가 패배 하였을 시
    virtual void ResetBossToDefault() = 0;

    // 전달용()
    virtual FOnBossStatUpdated& GetBossStatDelegate() = 0;
};