#include "BossBase/AnimNotify/AnimNotify_BossPhaseChangeEnd.h"
#include "BossBase/BossBase.h"

void UAnimNotify_BossPhaseChangeEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp) return;

	AActor* OwnerActor = MeshComp->GetOwner();
	if (!OwnerActor) return;

	if (ABossBase* Boss = Cast<ABossBase>(OwnerActor))
	{
		Boss->FinishPhaseChange();
	}
}
