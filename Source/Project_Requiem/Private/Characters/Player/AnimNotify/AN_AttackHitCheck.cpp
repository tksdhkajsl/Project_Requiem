// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Player/AnimNotify/AN_AttackHitCheck.h"
#include "Characters/Player/Character/PlayerCharacter.h"

void UAN_AttackHitCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComp->GetOwner());
		if (Player)
		{
			// "지금 칼 휘둘렀어! 데미지 판정해!"
			Player->ApplyAttackDamage();
		}
	}
}
