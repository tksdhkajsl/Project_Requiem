#include "Mics/NPC/Bonfire.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/Enemy/EnemySpawner.h"             //적 스포너
#include "Characters/Player/Character/PlayerCharacter.h"
#include "Stats/StatComponent.h"            // 스탯 컴포넌트
#include "Stats/Data/EFullStats.h"          // 스탯 Enum

#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "UI/Bonfire/BonfireWidget.h"

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

	FireEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FireEffect"));
	FireEffect->SetupAttachment(RootComponent);
	FireEffect->SetAutoActivate(false);

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

	if (FireSystem && FireEffect) FireEffect->SetAsset(FireSystem);
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
	switch (InteractionType) {
	case EInteractionType::Bonfire:
		return FText::FromString(TEXT("본파이어"));
	case EInteractionType::Merchant:
		return FText::FromString(TEXT("상인"));
	default:
		return FText::GetEmpty();
	}
}

void ABonfire::Interact_Implementation(APlayerCharacter* Caller)
{
	/*
	 *  25/12/17 코드 추가 : 천수호
	 *  추가 사유 : 나이아가라 이펙트 추가
	 */
	if (!Caller || bIsOpened) return;
	bIsOpened = !bIsOpened;
	if (FireEffect) FireEffect->Activate(true);

	// 1. 플레이어 회복
	HealPlayer(Caller);

	// 2. 적 리스폰 (월드의 모든 스포너 찾기)
	RespawnAllEnemies();

	// 3. 스탯 올리고,물약 살 수 있는 widget 띄우기 
	if (BonfireWidgetClass) {
		if (APlayerController* PC = GetWorld()->GetFirstPlayerController()) {
			BonfireWidget = CreateWidget<UBonfireWidget>(PC, BonfireWidgetClass);
			BonfireWidget->SetStatComponent(Caller->GetStatComponent());
			if (BonfireWidget) {
				BonfireWidget->AddToViewport();

				FInputModeUIOnly InputMode;
				InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
				PC->SetInputMode(InputMode);
				PC->bShowMouseCursor = true;

				BonfireWidget->OnBonfireClosed.AddDynamic(this, &ABonfire::HandleWidgetClosed);
			}
		}
	}
}
void ABonfire::HandleWidgetClosed()
{
	bIsOpened = false;
	if (FireEffect)  FireEffect->Deactivate();
}
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