// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/LastBossCharacter/AnimNotifyStateApplyDamage.h"
#include "Character/LastBossCharacter/LastBossCharacter.h"
#include "Stats/StatComponent.h"

void UAnimNotifyStateApplyDamage::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!LastBoss.IsValid())
	{
		LastBoss = Cast<ALastBossCharacter>(MeshComp->GetOwner());
	}
	if (LastBoss.IsValid())
	{
		if (Projectile)
		{
			DoSpawnProjectile(Projectile);
		}
	}
}

void UAnimNotifyStateApplyDamage::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	// 스폰된 Projectile 제거
	if (SpawnedProjectile.IsValid())
	{
		SpawnedProjectile->Destroy();
		SpawnedProjectile = nullptr;
	}
	Super::NotifyEnd(MeshComp, Animation, EventReference);
}

void UAnimNotifyStateApplyDamage::DoSpawnProjectile(TSubclassOf<AActor> InSpawnActor)
{
	if (LastBoss.IsValid() && InSpawnActor)
	{
		USceneComponent* SpawnComp = LastBoss->FindComponentByClass<USceneComponent>();
		UWorld* World = LastBoss->GetWorld();
		if (SpawnComp && World)
		{
			FTransform SpawnTransform = SpawnComp->GetComponentTransform();
			AActor* SpawnedActor = World->SpawnActor<AActor>(InSpawnActor, SpawnTransform);
			SpawnedProjectile = SpawnedActor;
		}
	}
}
