// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/AnimNotifyState_Invincibility.h"
#include "Player/PlayerCharacter.h"

void UAnimNotifyState_Invincibility::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	if (MeshComp && MeshComp->GetOwner())
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComp->GetOwner());

		if (Player)
		{
			// 무적 상태 켜기
			Player->SetInvincible(true);
		}
	}

}

void UAnimNotifyState_Invincibility::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	if (MeshComp && MeshComp->GetOwner())
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComp->GetOwner());

		if (Player)
		{
			// 무적 상태 끄기
			Player->SetInvincible(false);
		}
	}
}
