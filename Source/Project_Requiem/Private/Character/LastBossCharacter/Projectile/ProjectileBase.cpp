// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/LastBossCharacter/Projectile/ProjectileBase.h"
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
	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetRootComponent(CapsuleComponent);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(CapsuleComponent);

	Niagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara"));
	Niagara->SetupAttachment(CapsuleComponent);
}

// Called when the game starts or when spawned
void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
	OnActorBeginOverlap.AddDynamic(this, &AProjectileBase::OnPlayerBeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &AProjectileBase::OnPlayerEndOverlap);

	// 투사체 이동시 틱 타이머 설정
	if (!FMath::IsNearlyZero(TickRate))
		GetWorld()->GetTimerManager().SetTimer(
			TickTimerHandle,
			this,
			&AProjectileBase::MoveProjectile,
			TickRate,
			true
		);
}

void AProjectileBase::ActivateProjectile()
{
	bApplyDamageActive = true;
	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MoveProjectile();

	// 수명 타이머 설정
	if(LifeTime > 0.0f)
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

	// 수명 타이머 해제
	if(GetWorld())
		GetWorld()->GetTimerManager().ClearTimer(LifeTimerHandle);

	Destroy();
}

void AProjectileBase::OnPlayerBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!bApplyDamageActive && !OtherActor && OtherActor == OverlappedActor)
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
		}
	}

	ApplyDamageToPlayer(OtherActor, AttackDamage);

}

void AProjectileBase::OnPlayerEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	
}

void AProjectileBase::ApplyDamageToPlayer(AActor* OtherActor, float InDamage)
{
	if (!OtherActor || InDamage <= 0.0f)
		return;

	if (LastBoss.IsValid())
	{
		LastBoss->OnApplyDamage.Broadcast(InDamage);
	}

	// 싱글 히트 모드 활성화시 데미지를 준 후 투사체 비활성화
	if (SingleHitMode)
		DeactivateProjectile();
}

void AProjectileBase::MoveProjectile()
{
	if (TickMoveDistance <= 0.0f)
		return;

	FVector ForwardVector = GetActorForwardVector();

	FVector NewLocation = GetActorLocation() + ForwardVector * TickMoveDistance;
}

void AProjectileBase::ResetProjectileState()
{

}

