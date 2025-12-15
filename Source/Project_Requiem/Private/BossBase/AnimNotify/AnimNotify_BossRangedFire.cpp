
#include "BossBase/AnimNotify/AnimNotify_BossRangedFire.h"
#include "BossBase/BossBase.h"

void UAnimNotify_BossRangedFire::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp) return;

	// MeshComp의 Owner는 보통 “그 애니를 재생 중인 캐릭터(보스)”
	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	// Owner가 ABossBase라면, 소켓 기준 발사를 호출
	if (ABossBase* Boss = Cast<ABossBase>(Owner))
	{
		Boss->ApplyRangedAttackFromSocket(SocketName);
	}
}
