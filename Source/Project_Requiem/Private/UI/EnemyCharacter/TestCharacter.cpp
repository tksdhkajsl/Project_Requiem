#include "UI/EnemyCharacter/TestCharacter.h"
#include "Stats/StatComponent.h"

#pragma region 필요한 애들
#include "Components/WidgetComponent.h"
#include "UI/EnemyCharacter/EnemyCharacterHPWidget.h"
#pragma endregion

ATestCharacter::ATestCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	HPWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPWidgetComponent"));
	HPWidgetComponent->SetupAttachment(GetRootComponent());
	HPWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	HPWidgetComponent->SetDrawSize(FVector2D(100.f, 12.f));
	HPWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
	HPWidgetComponent->SetTickWhenOffscreen(true);
}
void ATestCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (UUserWidget* Widget = HPWidgetComponent->GetUserWidgetObject()) HPWidget = Cast<UEnemyCharacterHPWidget>(Widget);
	if (HPWidget) HPWidget->UpdateHP(StatComponent->GetStatCurrent(EFullStats::Health), 
									 StatComponent->GetStatMax(EFullStats::Health));

}
void ATestCharacter::ReceiveDamageTest()
{
	// 1. 맞았으면 피격몽타주 재생 

	StatComponent->ChangeStatCurrent(EFullStats::Health, -20.f);

	if (HPWidget) HPWidget->UpdateHP(StatComponent->GetStatCurrent(EFullStats::Health),
									 StatComponent->GetStatMax(EFullStats::Health));
	if (StatComponent->GetStatCurrent(EFullStats::Health) <= 0.f && HPWidgetComponent) HPWidgetComponent->SetVisibility(false);
}
void ATestCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HPWidgetComponent) return;
	if (StatComponent->GetStatCurrent(EFullStats::Health) <= 0.f) return;

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

	FVector CameraLocation;
	FRotator CameraRotation;
	PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

	// 거리 기반 표시
	float Distance = FVector::Dist(CameraLocation, GetActorLocation());
	bool bVisible = Distance < 2000.f;
	if (HPWidgetComponent->IsVisible() != bVisible)
		HPWidgetComponent->SetVisibility(bVisible);

	// Billboard
	FVector ToCamera = CameraLocation - HPWidgetComponent->GetComponentLocation();
	FRotator LookAt = ToCamera.Rotation();
	LookAt.Pitch = 0.f; // 수평 유지
	LookAt.Roll = 0.f;
	HPWidgetComponent->SetWorldRotation(LookAt);
}