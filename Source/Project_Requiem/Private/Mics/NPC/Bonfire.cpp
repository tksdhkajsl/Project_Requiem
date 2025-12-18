#include "Mics/NPC/Bonfire.h" // 경로가 다르다면 수정 필요 (예: "Actors/Props/Bonfire.h")
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/Enemy/EnemySpawner.h"     
#include "Characters/Player/Character/PlayerCharacter.h"
#include "Stats/StatComponent.h"
#include "Stats/Data/EFullStats.h"
#include "Blueprint/UserWidget.h"               // 위젯 생성용 헤더

ABonfire::ABonfire()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	InteractionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionBox"));
	InteractionBox->SetupAttachment(RootComponent);
	InteractionBox->SetBoxExtent(FVector(60.f, 60.f, 100.f));
	InteractionBox->SetRelativeLocation(FVector(0.f, 0.f, 80.f));

	// 충돌 설정: Visibility 채널을 막아야 플레이어의 라인 트레이스에 걸림
	InteractionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionBox->SetCollisionObjectType(ECC_WorldDynamic);
	InteractionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	InteractionBox->SetGenerateOverlapEvents(false);
}

// ========================================================
// 인터페이스 구현
// ========================================================
bool ABonfire::CanInteract_Implementation(const APlayerCharacter* Caller) const
{
	// 무조건 상호작용 가능
	return true;
}

FText ABonfire::GetInteractionText_Implementation(const APlayerCharacter* Caller) const
{
	return FText::FromString(TEXT("휴식하기"));
}

void ABonfire::Interact_Implementation(APlayerCharacter* Caller)
{
	UE_LOG(LogTemp, Log, TEXT("Bonfire Activated!"));

	// 1. 플레이어 회복
	HealPlayer(Caller);

	// 2. 적 리스폰 (월드의 모든 스포너 찾기)
	RespawnAllEnemies();

	// 3. UI 위젯 띄우기 (물약 구매, 레벨업 등)
	if (BonfireWidgetClass && Caller)
	{
		if (APlayerController* PC = Cast<APlayerController>(Caller->GetController()))
		{
			UUserWidget* BonfireWidget = CreateWidget<UUserWidget>(PC, BonfireWidgetClass);
			if (BonfireWidget)
			{
				BonfireWidget->AddToViewport();

				// 마우스 커서 보이게 하고, 게임 입력 막기
				PC->SetShowMouseCursor(true);
				FInputModeUIOnly InputMode;
				InputMode.SetWidgetToFocus(BonfireWidget->TakeWidget());
				PC->SetInputMode(InputMode);
			}
		}
	}
}

// ========================================================
// 내부 로직
// ========================================================
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

void ABonfire::HealPlayer(APlayerCharacter* Player)
{
	if (Player && Player->StatComponent)
	{
		// 최대 체력/스태미나 가져오기
		float MaxHP = Player->StatComponent->GetStatMax(EFullStats::Health);
		float MaxST = Player->StatComponent->GetStatMax(EFullStats::Stamina);

		// 현재 수치 가져오기
		float CurrentHP = Player->StatComponent->GetStatCurrent(EFullStats::Health);
		float CurrentST = Player->StatComponent->GetStatCurrent(EFullStats::Stamina);

		// 차이만큼 회복 (풀피/풀스태미나 만들기)
		Player->StatComponent->ChangeStatCurrent(EFullStats::Health, MaxHP - CurrentHP);
		Player->StatComponent->ChangeStatCurrent(EFullStats::Stamina, MaxST - CurrentST);

		UE_LOG(LogTemp, Log, TEXT("Player Fully Healed"));
	}
}