// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Player/AnimNotify/ANS_ComboWindow.h"
#include "Characters/Player/Character/PlayerCharacter.h"

void UANS_ComboWindow::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComp->GetOwner());
		if (Player)
		{
			// 플레이어가 "다음 공격"을 예약해놨는지 확인
			// 예약되어 있다면 즉시 콤보 실행
			Player->CheckComboInput();
		}
	}
}
