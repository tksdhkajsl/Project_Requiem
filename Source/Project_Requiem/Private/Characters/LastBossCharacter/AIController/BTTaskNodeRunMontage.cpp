// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/LastBossCharacter/AIController/BTTaskNodeRunMontage.h"
#include "GameFramework/Pawn.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/LastBossCharacter/LastBossCharacter.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"

void UBTTaskNodeRunMontage::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);


}

EBTNodeResult::Type UBTTaskNodeRunMontage::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!LastBoss.IsValid())
	{
		AAIController* AICon = OwnerComp.GetAIOwner();
		if (!AICon)
		{
			UE_LOG(LogTemp, Warning, TEXT("비헤이비어트리의 AIController가 없습니다"));
			return EBTNodeResult::Failed;
		}
		APawn* Pawn = AICon->GetPawn();
		if (!Pawn)
		{
			UE_LOG(LogTemp, Warning, TEXT("비헤이비어트리의 Pawn이 없습니다"));
			return EBTNodeResult::Failed;
		}
		LastBoss = Cast<ALastBossCharacter>(Pawn);
		if (!LastBoss.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("비헤이비어트리에서 LastBoss를 Cast실패했습니다"));
			return EBTNodeResult::Failed;
		}
	}
	if (!BlackBoardComp.IsValid())
	{
		BlackBoardComp = OwnerComp.GetBlackboardComponent();
	}

	if (LastBoss.IsValid() && BlackBoardComp.IsValid())
	{
		// 블랙보드 키값 저장
		int32 RandomMontageNum = BlackBoardComp->GetValueAsInt(FName("MontageNumber"));
		int32 Phase = BlackBoardComp->GetValueAsInt(FName("Phase"));

		// 실행할 몽타주
		UAnimMontage* MontageToPlay = nullptr;
		if (Phase == 1)			// 보스의 페이즈1 몽타주 랜덤 출력
		{
			// 보스의 몽타주배열 저장 
			const TArray<UAnimMontage*> MontageArr = LastBoss->GetPhaseOnePatterns();
			// 랜덤으로 나온 키값의 몽타주가 있다면 실행할 몽타주에 저장
			if (MontageArr.IsValidIndex(RandomMontageNum))
				MontageToPlay = MontageArr[RandomMontageNum];
		}
		else if (Phase == 2)	// 보스의 페이즈2 몽타주 랜덤 출력
		{
			const TArray<UAnimMontage*> MontageArr = LastBoss->GetPhaseTwoPatterns();
			if (MontageArr.IsValidIndex(RandomMontageNum))
				MontageToPlay = MontageArr[RandomMontageNum];
		}

		// 실행할 몽타주가 없을 때
		if (!MontageToPlay)
		{
			UE_LOG(LogTemp, Warning, TEXT("실행할 몽타주가 없습니다"));
			return EBTNodeResult::Failed;
		}

		// 보스의 애님인스턴스 저장
		UAnimInstance* AnimInst = LastBoss->GetMesh()->GetAnimInstance();
		if (!AnimInst)
		{
			UE_LOG(LogTemp, Warning, TEXT("보스의 애님인스턴스를 가져오지 못했습니다"));
			return EBTNodeResult::Failed;
		}
		
		const float PlayRate = 1.0f;
		float Duration = AnimInst->Montage_Play(MontageToPlay, PlayRate);
		if (Duration > 0.0f)
		{
			FOnMontageEnded OnMontageEnded;
			OnMontageEnded.BindUObject(this, &UBTTaskNodeRunMontage::MontageEnded);
			AnimInst->Montage_SetEndDelegate(OnMontageEnded, MontageToPlay);

			CachedOwnerComp = &OwnerComp;

			LastBoss->OnLastBossChangedPhase.RemoveDynamic(this, &UBTTaskNodeRunMontage::NextPhase);
			LastBoss->OnLastBossChangedPhase.AddDynamic(this, &UBTTaskNodeRunMontage::NextPhase);

			// 몽타주가 실행될 경우 비동기(지연)상태 반환
			return EBTNodeResult::InProgress;
		}
	}
	return EBTNodeResult::Failed;
}

void UBTTaskNodeRunMontage::MontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!CachedOwnerComp.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("CachedOwnerComp에 저장된 캐시가 없습니다"));
		return;
	}

	// 중간에 몽타주가 멈췄다면 Failed
	EBTNodeResult::Type Result = bInterrupted ? EBTNodeResult::Failed : EBTNodeResult::Succeeded;

	// ExecuteTask의 InProgress상태 풀고 Result결과값 비헤이비어 트리에 전달
	FinishLatentTask(*CachedOwnerComp.Get(), Result);

	// 캐시 해제
	CachedOwnerComp = nullptr;
}

void UBTTaskNodeRunMontage::NextPhase()
{
	if (LastBoss.IsValid())
	{
		LastBoss->OnLastBossChangedPhase.RemoveDynamic(this, &UBTTaskNodeRunMontage::NextPhase);
	}

	FinishLatentTask(*CachedOwnerComp.Get(), EBTNodeResult::Failed);
	
	CachedOwnerComp = nullptr;
}

