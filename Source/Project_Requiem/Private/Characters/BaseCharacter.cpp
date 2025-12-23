#include "Characters/BaseCharacter.h"

#include "Stats/StatComponent.h"
<<<<<<< HEAD:Source/Project_Requiem/Private/Characters/BaseCharacter.cpp
#include <Kismet/GameplayStatics.h>
=======

#pragma region 캡쳐 컴포넌트 관련
#include "Components/SceneCaptureComponent2D.h"
#pragma endregion
>>>>>>> 2bd40acd771b463067b062a5a771101532b23eb1:Source/Project_Requiem/Private/Character/BaseCharacter.cpp

//---------------기본 생성-------------------
ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

#pragma region 컴포넌트 관련 초기화
	StatComponent = CreateDefaultSubobject<UStatComponent>(TEXT("StatComponent"));
#pragma endregion
<<<<<<< HEAD:Source/Project_Requiem/Private/Characters/BaseCharacter.cpp
=======

#pragma region 캡쳐 컴포넌트 관련
	PortraitCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("PortraitCapture"));
	PortraitCaptureComponent->SetupAttachment(RootComponent);
	PortraitCaptureComponent->SetRelativeLocation(FVector(150.f, 0.f, 70.f));
	PortraitCaptureComponent->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));
	PortraitCaptureComponent->bCaptureEveryFrame = false;
	PortraitCaptureComponent->bAlwaysPersistRenderingState = true;
	PortraitCaptureComponent->CaptureSource = ESceneCaptureSource::SCS_SceneColorHDR;
#pragma endregion
>>>>>>> 2bd40acd771b463067b062a5a771101532b23eb1:Source/Project_Requiem/Private/Character/BaseCharacter.cpp
}
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

#pragma region 스탯 컴포넌트
	StatComponent->InitializeStatsComponent();
#pragma endregion
<<<<<<< HEAD:Source/Project_Requiem/Private/Characters/BaseCharacter.cpp
=======

#pragma region 캡쳐 컴포넌트 관련
	if (PortraitCaptureComponent) {
		TArray<AActor*> ActorsToShow;
		ActorsToShow.Add(this);
		PortraitCaptureComponent->ShowOnlyActors = ActorsToShow;
		PortraitCaptureComponent->bCaptureEveryFrame = false;
	}
#pragma endregion
>>>>>>> 2bd40acd771b463067b062a5a771101532b23eb1:Source/Project_Requiem/Private/Character/BaseCharacter.cpp
}
//---------------스탯 컴포넌트 관련-------------------

//---------------캐릭터 기본 함수들-------------------
void ABaseCharacter::ReceiveDamage(float DamageAmount)
{
	if (!StatComponent) return;
	StatComponent->ChangeStatCurrent(EFullStats::Health, -DamageAmount);

	if (StatComponent->GetStatCurrent(EFullStats::Health) <= 0.f) Die();
}
void ABaseCharacter::ApplyHeal(float HealAmount)
{
	if (!StatComponent) return;

	StatComponent->ChangeStatCurrent(EFullStats::Health, HealAmount);
}
void ABaseCharacter::Die()
{
	//double Exp = StatComponent->GetCurrentExp() * 0.1f;
	//AddExp(-Exp);

	// TODO: 애니메이션, 사망 상태 처리
	UE_LOG(LogTemp, Warning, TEXT("%s died!"), *GetName());
	// 예: Destroy() 또는 상태 플래그 변경
}
void ABaseCharacter::AddExp(float Amount)
{
	StatComponent->AddCurrentExp(Amount);
}
// ─────────────────────────
// Movement
// ─────────────────────────
void ABaseCharacter::Move(const FVector& Direction, float Value)
{
	if (FMath::IsNearlyZero(Value) || Direction.IsZero())
		return;

	AddMovementInput(Direction, Value);
}
void ABaseCharacter::StartSprint()
{
	if (!StatComponent) return;

}
bool ABaseCharacter::CanSprint() const
{
	if (!StatComponent) return false;
	if (IsStunned()) return false;
	if (StatComponent->GetStatCurrent(EFullStats::Stamina) <= 0.f) return false;


	// TODO: 기타 조건 (과부하 상태, 특정 디버프 등)

	return true;
}
void ABaseCharacter::StopSprint()
{
	if (!StatComponent) return;
}
void ABaseCharacter::Jump()
{
	UE_LOG(LogTemp, Warning, TEXT("%s Jump"), *GetName());
	Super::Jump();
}
void ABaseCharacter::StopJumping()
{
	UE_LOG(LogTemp, Warning, TEXT("%s StopJump"), *GetName());
	Super::StopJumping();
}
void ABaseCharacter::StopMovement()
{
	UE_LOG(LogTemp, Warning, TEXT("%s StopMovement"), *GetName());
}
// ─────────────────────────
// Combat & Actions
// ─────────────────────────
void ABaseCharacter::Attack(AActor* HitActor, float AttackDamage)
{
	if (!IsValid(HitActor)) return;

	UGameplayStatics::ApplyDamage(
		HitActor,
		AttackDamage,
		GetController(),
		this,
		UDamageType::StaticClass()
	);

	UE_LOG(LogTemp, Warning, TEXT("%s Attack()"), *GetName());
}
void ABaseCharacter::StopAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("%s StopAttack()"), *GetName());
}
bool ABaseCharacter::CanAttack() const
{
	// 필요시 스턴 상태, 마나, 쿨타임 등 체크 가능
	return true;
}
void ABaseCharacter::Interact()
{
	UE_LOG(LogTemp, Warning, TEXT("%s Interact()"), *GetName());
}
// ─────────────────────────
// Damage & Death
// ─────────────────────────
float ABaseCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float AppliedDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (AppliedDamage > 0.f) {
		LastHitInstigator = EventInstigator;
		ReceiveDamage(AppliedDamage);
	}

	return AppliedDamage;
}
void ABaseCharacter::OnDeathFinished()
{
	UE_LOG(LogTemp, Warning, TEXT("%s OnDeathFinished()"), *GetName());
}
// ─────────────────────────
// Status (Stun 등 CC)
// ─────────────────────────
void ABaseCharacter::SetStunned(bool bStunned)
{
	UE_LOG(LogTemp, Warning, TEXT("%s Stunned = %s"), *GetName(), bStunned ? TEXT("true") : TEXT("false"));


}
bool ABaseCharacter::IsStunned() const
{
	return false;
}