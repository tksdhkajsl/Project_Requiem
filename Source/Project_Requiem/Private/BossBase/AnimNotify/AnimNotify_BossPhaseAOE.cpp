// Fill out your copyright notice in the Description page of Project Settings.


#include "BossBase/AnimNotify/AnimNotify_BossPhaseAOE.h"
#include "BossBase/BossBase.h"


void UAnimNotify_BossPhaseAOE::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp) return;

	if (ABossBase* Boss = Cast<ABossBase>(MeshComp->GetOwner()))
	{
		Boss->ApplyPhaseChangeAOE();
	}
}
