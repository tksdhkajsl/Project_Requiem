// Fill out your copyright notice in the Description page of Project Settings.


#include "BossBase/Projectile/BossProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABossProjectile::ABossProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SetRootComponent(Collision);

	Collision->InitSphereRadius(12.0f);

	Collision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Collision->SetCollisionObjectType(ECC_WorldDynamic);

	Collision->SetCollisionResponseToAllChannels(ECR_Ignore);

	Collision->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	Collision->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);

	Collision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	Collision->SetNotifyRigidBodyCollision(true);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = Collision;
	ProjectileMovement->InitialSpeed = 1200.0f;
	ProjectileMovement->MaxSpeed = 1200.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.0f;
}

// Called when the game starts or when spawned
void ABossProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	if (LifeTime > 0.0f)
	{
		SetLifeSpan(LifeTime);
	}

	if (Collision)
	{
		Collision->OnComponentBeginOverlap.AddDynamic(this, &ABossProjectile::OnCollisionBeginOverlap);
		Collision->OnComponentHit.AddDynamic(this, &ABossProjectile::OnCollisionHit);
	}

	if (AActor* OwnerActor = GetOwner())
	{
		Collision->IgnoreActorWhenMoving(OwnerActor, true);
	}
}

// Called every frame
void ABossProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABossProjectile::InitProjectile(float InDamage, AController* InInstigatorController)
{
	Damage = InDamage;

	if (InInstigatorController)
	{
		SetInstigator(InInstigatorController->GetPawn());
	}
}

void ABossProjectile::OnCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bHitOnce)
	{
		return;
	}

	if (!OtherActor || OtherActor == this)
	{
		return;
	}

	if (OtherActor == GetOwner())
	{
		return;
	}

	HandleImpactAndDestroy(OtherActor);
}

void ABossProjectile::OnCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (bHitOnce)
	{
		return;
	}

	if (!OtherActor || OtherActor == this)
	{
		bHitOnce = true;
		Destroy();
		return;
	}

	if (OtherActor == GetOwner())
	{
		return;
	}

	HandleImpactAndDestroy(OtherActor);
}

void ABossProjectile::HandleImpactAndDestroy(AActor* HitActor)
{
	bHitOnce = true;

	AController* InstigatorController = nullptr;
	if (APawn* InstigatorPawn = GetInstigator())
	{
		InstigatorController = InstigatorPawn->GetController();
	}

	UGameplayStatics::ApplyDamage(
		HitActor,
		Damage,
		InstigatorController,
		GetOwner() ? GetOwner() : this,
		nullptr
	);

	Destroy();
}

