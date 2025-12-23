// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/Player/AnimNotify/AnimPlayerNotifyState_Invincibility.h"
#include "Characters/Player/Character/PlayerCharacter.h"

void UAnimPlayerNotifyState_Invincibility::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	if (MeshComp && MeshComp->GetOwner())
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComp->GetOwner());

		if (Player)
		{
			UE_LOG(LogTemp, Log, TEXT("무적 : true"));
			// 무적 상태 켜기
			Player->SetInvincible(true);
		}
	}

}

void UAnimPlayerNotifyState_Invincibility::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	if (MeshComp && MeshComp->GetOwner())
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComp->GetOwner());

		if (Player)
		{
			UE_LOG(LogTemp, Log, TEXT("무적 : false"));
			// 무적 상태 끄기
			Player->SetInvincible(false);
		}
	}
}
