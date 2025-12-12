// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/LastBossCharacter/Projectile/ProjectileBase.h"
#include "Components/CapsuleComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"

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
	
}

void AProjectileBase::ActivateProjectile()
{
}

void AProjectileBase::DeactivateProjectile()
{
}

bool AProjectileBase::IsActiveProjectile()
{
	return false;
}

void AProjectileBase::ApplyDamageToActor()
{
}

void AProjectileBase::MoveProjectile()
{
}

void AProjectileBase::ResetProjectileState()
{
}

