#include "Mics/NPC/Bonfire.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/Enemy/EnemySpawner.h"
#include "Characters/Player/Character/PlayerCharacter.h"
#include "Stats/StatComponent.h"          
#include "Stats/Data/EFullStats.h"        

#include "UI/Bonfire/BonfireWidget.h" /// 코드 변경 : 12/24 (#include "Blueprint/UserWidget.h")

// ========================================================
// 언리얼 생성
// ========================================================
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
	InteractionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	InteractionBox->SetGenerateOverlapEvents(true);
}
void ABonfire::BeginPlay()
{
	Super::BeginPlay();

}
// ========================================================
// HUD 위젯 클래스
// ========================================================
bool ABonfire::CanInteract_Implementation(const APlayerCharacter* Caller) const
{
	return true;
}

FText ABonfire::GetInteractionText_Implementation(const APlayerCharacter* Caller) const
{
	return FText::FromString(TEXT("휴식하기"));
}

void ABonfire::Interact_Implementation(APlayerCharacter* Caller)
{
	/// 코드 추가 : 12/24 (해당 UI 두번 열리지 않게 방어코드)
	if (!Caller || bIsOpened) return;
	bIsOpened = !bIsOpened;

	// 1. 플레이어 회복
	HealPlayer(Caller);

	// 2. 플레이어 리스폰 지점 갱신
	// 화톳불 위치 + 약간 앞쪽으로 설정 (화톳불 모델에 끼임 방지)
	FVector NewSpawnLoc = GetActorLocation() + (GetActorForwardVector() * 100.0f);
	NewSpawnLoc.Z = 2.0f; // 바닥에 파묻히지 않게 살짝 띄움

	Caller->SetSpawnLocation(NewSpawnLoc);

	// 3. 적 리스폰 (월드의 모든 스포너 찾기)
	RespawnAllEnemies();

	// 4. 스탯 올리고,물약 살 수 있는 widget 띄우기 
	if (BonfireWidgetClass) {
		if (APlayerController* PC = Cast<APlayerController>(Caller->GetController())) {
			BonfireWidget = CreateWidget<UBonfireWidget>(PC, BonfireWidgetClass);
			BonfireWidget->SetStatComponent(Caller->GetStatComponent());
			if (BonfireWidget) {
				BonfireWidget->AddToViewport();

				PC->SetShowMouseCursor(true);
				FInputModeUIOnly InputMode;
				InputMode.SetWidgetToFocus(BonfireWidget->TakeWidget());
				PC->SetInputMode(InputMode);

				BonfireWidget->OnBonfireClosed.AddDynamic(this, &ABonfire::HandleWidgetClosed);
			}
		}
	}
}
void ABonfire::HandleWidgetClosed()
{
	bIsOpened = false;
}
// ========================================================
// 내부 로직
// ========================================================
void ABonfire::RespawnAllEnemies()
{
	TArray<AActor*> FoundSpawners;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemySpawner::StaticClass(), FoundSpawners);

	for (AActor* Actor : FoundSpawners) {
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