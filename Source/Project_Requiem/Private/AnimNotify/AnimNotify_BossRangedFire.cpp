// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/AnimNotify_BossRangedFire.h"
#include "BossBase/BossBase.h"

void UAnimNotify_BossRangedFire::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	if (ABossBase* Boss = Cast<ABossBase>(Owner))
	{
		Boss->ApplyRangedAttack();
	}

}
