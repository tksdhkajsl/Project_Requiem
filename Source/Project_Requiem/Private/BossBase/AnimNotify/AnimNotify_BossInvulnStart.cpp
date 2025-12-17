// Fill out your copyright notice in the Description page of Project Settings.


#include "BossBase/AnimNotify/AnimNotify_BossInvulnStart.h"
#include "BossBase/BossBase.h"

void UAnimNotify_BossInvulnStart::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp) return;
	if (ABossBase* Boss = Cast<ABossBase>(MeshComp->GetOwner()))
	{
		Boss->StartCurrentPatternInvulnerability();
	}
}
