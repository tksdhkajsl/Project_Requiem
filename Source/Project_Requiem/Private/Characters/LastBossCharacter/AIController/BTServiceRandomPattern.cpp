// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/LastBossCharacter/AIController/BTServiceRandomPattern.h"
#include "Characters/LastBossCharacter/LastBossCharacter.h"

void UBTServiceRandomPattern::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	if (!LastBoss.IsValid())
		LastBoss = Cast<ALastBossCharacter>(OwnerComp.GetOwner());

	if (LastBoss->GetBossPhase() == 1)			// 보스의 페이즈1 몽타주 랜덤 출력
	{
		int32 Index = FMath::RandRange(0, LastBoss->GetPhaseOnePatterns().Num() - 1);	// 배열 내 몽타주의 수 까지 랜덤한 수 뽑기

		LastBoss->PlayAnimMontage(LastBoss->GetPhaseOnePatterns()[Index]);
	}
	else if (LastBoss->GetBossPhase() == 2)		// 보스의 페이즈2 몽타주 랜덤 출력
	{
		int32 Index = FMath::RandRange(0, LastBoss->GetPhaseTwoPatterns().Num() - 1);

		LastBoss->PlayAnimMontage(LastBoss->GetPhaseTwoPatterns()[Index]);
	}
}
