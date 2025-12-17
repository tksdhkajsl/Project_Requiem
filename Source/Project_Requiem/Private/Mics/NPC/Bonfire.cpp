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
	PrimaryActorTick.bCanEverTick = false;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	InteractionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionBox"));
	InteractionBox->SetupAttachment(RootComponent);

	InteractionBox->SetBoxExtent(FVector(60.f, 60.f, 100.f));
	InteractionBox->SetRelativeLocation(FVector(0.f, 0.f, 80.f));

	InteractionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionBox->SetCollisionObjectType(ECC_WorldDynamic);
	InteractionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	InteractionBox->SetGenerateOverlapEvents(false);
}

// ========================================================
// HUD 위젯 클래스
// ========================================================
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
	
	// 3. 스탯 올리고,물약 살 수 있는 widget 띄우기 

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

		// 상태 ,물약 관련 로직도 여기에 추가 가능
		UE_LOG(LogTemp, Log, TEXT("Player Fully Healed"));
	}
}



