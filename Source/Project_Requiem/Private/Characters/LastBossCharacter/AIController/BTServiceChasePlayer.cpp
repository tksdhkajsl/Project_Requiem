// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/LastBossCharacter/AIController/BTServiceChasePlayer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

void UBTServiceChasePlayer::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

}

void UBTServiceChasePlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if (!BlackBoardComp.IsValid())
	{
		BlackBoardComp = OwnerComp.GetBlackboardComponent();
	}

	UWorld* World = OwnerComp.GetWorld();
	if (!World)
		return;
	APawn* Player = UGameplayStatics::GetPlayerPawn(World, 0);

	if (World && Player)
	{
		if (BlackBoardComp.IsValid())
		{
			// 에디터에서 선택된 키값에 플레이어 위치값 저장
			BlackBoardComp->SetValueAsVector(TargetPlayerLocationKey.SelectedKeyName, Player->GetActorLocation());

			// 플레이어 위치 로그 표시
			FVector StoredLocation = BlackBoardComp->GetValueAsVector(TargetPlayerLocationKey.SelectedKeyName);
			//UE_LOG(LogTemp, Log, TEXT("PlayerLocation Key Value : %s"), *StoredLocation.ToString());
			//UE_LOG(LogTemp, Log, TEXT("PlayerLocation : %s"), *Player->GetActorLocation().ToString());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Blackboard가 없습니다!!!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("World와 Player가 없습니다!!!"));
	}
	
}
