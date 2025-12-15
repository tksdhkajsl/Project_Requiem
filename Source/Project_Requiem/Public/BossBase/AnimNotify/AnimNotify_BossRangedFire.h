#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_BossRangedFire.generated.h"

UCLASS()
class PROJECT_REQUIEM_API UAnimNotify_BossRangedFire : public UAnimNotify
{
	GENERATED_BODY()

public:
	// 애니메이션 재생 중 Notify 타이밍에 호출됨
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

protected:
	// 어느 소켓에서 쏠지(몽타주 노티파이 디테일에서 설정)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	FName SocketName = NAME_None;
};
