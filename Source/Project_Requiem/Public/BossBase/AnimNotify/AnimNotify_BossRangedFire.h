#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_BossRangedFire.generated.h"

UCLASS()
class PROJECT_REQUIEM_API UAnimNotify_BossRangedFire : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

protected:
	// 어느 소켓에서 쏠지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	FName SocketName = NAME_None;
};
