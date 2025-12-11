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
		if (bCanAttack == true)
		{
			LastBoss->ApplyDamage(LastBoss->StatComponent->PhyAtt);
			bCanAttack = false;
		}
	}
}

void UAnimNotifyStateApplyDamage::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	bCanAttack = true;
	Super::NotifyEnd(MeshComp, Animation, EventReference);
}
