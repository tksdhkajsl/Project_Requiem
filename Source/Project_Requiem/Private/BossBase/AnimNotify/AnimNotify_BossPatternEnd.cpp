// Fill out your copyright notice in the Description page of Project Settings.


#include "BossBase/AnimNotify/AnimNotify_BossPatternEnd.h"
#include "BossBase/BossBase.h"

void UAnimNotify_BossPatternEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp) return;

	AActor* OwnerActor = MeshComp->GetOwner();
	if (!OwnerActor) return;

	ABossBase* Boss = Cast<ABossBase>(OwnerActor);
	if (!Boss) return;

	// 패턴 종료
	Boss->FinishCurrentPattern();
}
