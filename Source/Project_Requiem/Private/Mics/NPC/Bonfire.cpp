// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/Enemy/EnemySpawner.h"             //적 스포너
#include "Characters/Player/Character/PlayerCharacter.h"
#include "Stats/StatComponent.h"            // 스탯 컴포넌트
#include "Stats/Data/EFullStats.h"          // 스탯 Enum
#include "Mics/NPC/Bonfire.h"

// Sets default values
ABonfire::ABonfire()
{ 	

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	InteractionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionBox"));
	InteractionBox->SetupAttachment(RootComponent);
	InteractionBox->SetBoxExtent(FVector(100.f, 100.f, 100.f));

	// 인터렉션 트레이스 채널 설정 (PlayerCharacter.cpp의 TraceForInteraction 참고)
	InteractionBox->SetCollisionProfileName(TEXT("BlockAllDynamic"));

}

bool ABonfire::CanInteract_Implementation(const APlayerCharacter* Caller) const
{
	return false;
}

FText ABonfire::GetInteractionText_Implementation(const APlayerCharacter* Caller) const
{
	return FText();
}

void ABonfire::Interact_Implementation(APlayerCharacter* Caller)
{
	UE_LOG(LogTemp, Log, TEXT("Bonfire Activated!"));

	// 1. 플레이어 회복
	HealPlayer(Caller);

	// 2. 적 리스폰 (월드의 모든 스포너 찾기)
	RespawnAllEnemies();
}

void ABonfire::RespawnAllEnemies()
{
	TArray<AActor*> FoundSpawners;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemySpawner::StaticClass(), FoundSpawners);

	for (AActor* Actor : FoundSpawners)
	{
		if (AEnemySpawner* Spawner = Cast<AEnemySpawner>(Actor))
		{
			Spawner->RespawnEnemy();
		}
	}
	UE_LOG(LogTemp, Log, TEXT("All Enemies Respawned"));
}

void ABonfire::HealPlayer(AActor* PlayerActor)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(PlayerActor);
	if (Player && Player->StatComponent)
	{
		// 스탯 컴포넌트에서 최대 체력/스태미나 가져오기
		float MaxHP = Player->StatComponent->GetStatMax(EFullStats::Health);
		float MaxST = Player->StatComponent->GetStatMax(EFullStats::Stamina);

		// 현재 수치를 최대치로 설정 (ChangeStatCurrent 대신 직접 설정하거나 회복량 계산)
		// StatComponent 구조상 Current 변수에 직접 접근이 어려우므로, ChangeStatCurrent로 차이만큼 회복
		float CurrentHP = Player->StatComponent->GetStatCurrent(EFullStats::Health);
		float CurrentST = Player->StatComponent->GetStatCurrent(EFullStats::Stamina);

		Player->StatComponent->ChangeStatCurrent(EFullStats::Health, MaxHP - CurrentHP);
		Player->StatComponent->ChangeStatCurrent(EFullStats::Stamina, MaxST - CurrentST);

		// 상태 이상 해제나 포션 리필 로직도 여기에 추가 가능
		UE_LOG(LogTemp, Log, TEXT("Player Fully Healed"));
	}
}



