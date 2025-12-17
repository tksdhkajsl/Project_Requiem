// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Player/AnimNotify/AN_CheckCombo.h"
#include "Characters/Player/Character/PlayerCharacter.h"

UAN_CheckCombo::UAN_CheckCombo()
{
	bIsNativeBranchingPoint = true; // 몽타주 분기점으로 사용할 때 성능 최적화
}

void UAN_CheckCombo::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		// 플레이어 캐릭터로 캐스팅해서 함수 호출
		APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComp->GetOwner());
		if (Player)
		{
			// "다음 콤보 입력이 들어왔었는지 확인해줘!"
			Player->CheckComboInput();
		}
	}
}
