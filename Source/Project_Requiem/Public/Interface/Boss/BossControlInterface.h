#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BossControlInterface.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnBossStatUpdated, float, CurHP, float, MaxHP, FText, Name);

UINTERFACE(MinimalAPI)
class UBossControlInterface : public UInterface 
{
    GENERATED_BODY()
};

class PROJECT_REQUIEM_API IBossControlInterface
{
    GENERATED_BODY()

public:
    // 문이 열릴 때 보스에게 내릴 명령
    virtual void ActivateBossBattle() = 0;

    // 보스에게 플레이어가 패배 하였을 시
    virtual void ResetBossToDefault() = 0;

    // 보스 스텟 업데이트
    virtual FOnBossStatUpdated& GetBossStatDelegate() = 0;
};