// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/LastBossCharacter/AIController/BTServiceSettingPhase.h"
#include "Characters/LastBossCharacter/LastBossCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UBTServiceSettingPhase::UBTServiceSettingPhase()
{
	bNotifyBecomeRelevant = true;
}

void UBTServiceSettingPhase::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	if (!LastBoss.IsValid())
	{
		AAIController* AICon = OwnerComp.GetAIOwner();
		if (AICon)
		{
			APawn* Pawn = AICon->GetPawn();
			LastBoss = Cast<ALastBossCharacter>(Pawn);
		}
	}
	if (!BlackBoardComp.IsValid())
	{
		BlackBoardComp = OwnerComp.GetBlackboardComponent();
	}

	if (LastBoss.IsValid())
	{
		// 블랙보드의 키값에 보스 캐릭터의 페이즈 저장
		BlackBoardComp->SetValueAsInt(PhaseKey.SelectedKeyName, LastBoss->GetBossPhase());

		// 블랙보드의 키값에 보스 캐릭터가 출력할 몽타주배열의 번호 저장
		if (LastBoss->GetBossPhase() == 1)
		{
			int32 PhaseOneIndex = FMath::RandRange(0, LastBoss->GetPhaseOnePatterns().Num() - 1);
			BlackBoardComp->SetValueAsInt(MontageNumberKey.SelectedKeyName, PhaseOneIndex);
		}
		else if (LastBoss->GetBossPhase() == 2)
		{
			int32 PhaseTwoIndex = FMath::RandRange(0, LastBoss->GetPhaseTwoPatterns().Num() - 1);
			BlackBoardComp->SetValueAsInt(MontageNumberKey.SelectedKeyName, PhaseTwoIndex);
		}
	}
}
