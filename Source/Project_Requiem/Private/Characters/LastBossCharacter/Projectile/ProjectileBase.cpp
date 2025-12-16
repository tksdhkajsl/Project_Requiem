// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/LastBossCharacter/Projectile/ProjectileBase.h"
#include "Components/CapsuleComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Stats/StatComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AProjectileBase::AProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	SetRootComponent(CapsuleComponent);
	CapsuleComponent->SetCollisionProfileName(FName("OverlapAll"), true);
	CapsuleComponent->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(CapsuleComponent);
	MeshComponent->SetCollisionProfileName(FName("OverlapAll"),false);
	MeshComponent->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;

	Niagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara"));
	Niagara->SetupAttachment(CapsuleComponent);

	
}

// Called when the game starts or when spawned
void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
	ActivateProjectile();

	OnActorBeginOverlap.AddDynamic(this, &AProjectileBase::OnPlayerBeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &AProjectileBase::OnPlayerEndOverlap);

	// 틱 타이머
	GetWorld()->GetTimerManager().ClearTimer(TickTimerHandle);
	if (!FMath::IsNearlyZero(TickRate))
		GetWorld()->GetTimerManager().SetTimer(
			TickTimerHandle,
			this,
			&AProjectileBase::ActivateTick,
			TickRate,
			true
		);
}

void AProjectileBase::ActivateTick()
{
	MoveProjectile();
}

void AProjectileBase::ActivateProjectile()
{
	bApplyDamageActive = true;
	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	// 투사체 지속시간 설정
	if (LifeTime > 0.0f)
	{
		GetWorld()->GetTimerManager().ClearTimer(LifeTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(
			LifeTimerHandle,
			this,
			&AProjectileBase::DeactivateProjectile,
			LifeTime,
			false
		);
	}
}

void AProjectileBase::DeactivateProjectile()
{
	bApplyDamageActive = false;
	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 타이머 초기화
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(LifeTimerHandle);
		GetWorld()->GetTimerManager().ClearTimer(TickTimerHandle);
	}

	UE_LOG(LogTemp, Log, TEXT("투사체 지속시간에 의해 스폰된 액터가 삭제되었습니다"));
	Destroy();
}

void AProjectileBase::OnPlayerBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	// 유효성 검사
	if (!bApplyDamageActive || !OtherActor || OtherActor == OverlappedActor)
		return;

	if (!LastBoss.IsValid())
	{
		LastBoss = Cast<ALastBossCharacter>(GetOwner());
	}

	float AttackDamage = 0.0f;
	if (LastBoss.IsValid())
	{
		if (UStatComponent* BossStat = LastBoss->GetStatComponent())
		{
			AttackDamage = BossStat->PhyAtt * Damagemagnification;
			UE_LOG(LogTemp, Log, TEXT("AttackDamage : %f"), AttackDamage);

			ApplyDamageToPlayer(OtherActor, AttackDamage);
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("OnPlayerBeginOverlap 실행실패"));
	}
}

void AProjectileBase::OnPlayerEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{

}

void AProjectileBase::ApplyDamageToPlayer(AActor* OtherActor, float InDamage)
{
	if (!OtherActor || (InDamage <= 0.0f))
		return;

	APawn* Target = Cast<APawn>(OtherActor);
	if (!Target || !Target->IsPlayerControlled())
		return;


	LastBoss->Attack(OtherActor, InDamage);

	// SingleHitMode 활성화시 투사체가 데미지를 준 후 비활성화
	if (SingleHitMode)
		DeactivateProjectile();
}

void AProjectileBase::MoveProjectile()
{
	if (TickMoveDistance <= 0.0f)
		return;

	FVector ForwardVector = GetActorForwardVector();
	FVector NextLocation = GetActorLocation() + ForwardVector * TickMoveDistance;
	FVector NewLocation = FMath::VInterpTo(
		GetActorLocation(),
		NextLocation,
		TickRate,
		TickMoveDistance
	);
	SetActorLocation(NewLocation);
}

void AProjectileBase::ResetProjectileState()
{

}

