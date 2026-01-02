// BossProjectile.cpp
#include "BossBase/Projectile/BossProjectile.h"

#include "Components/ShapeComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

ABossProjectile::ABossProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SetRootComponent(Collision);

	if (USphereComponent* Sphere = Cast<USphereComponent>(Collision))
	{
		Sphere->InitSphereRadius(60.0f);
	}


	Collision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Collision->SetCollisionObjectType(ECC_WorldDynamic);

	Collision->SetCollisionResponseToAllChannels(ECR_Ignore);

	Collision->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Ignore);
	Collision->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);

	Collision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	Collision->SetNotifyRigidBodyCollision(true);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = Collision;
	ProjectileMovement->InitialSpeed = 1200.0f;
	ProjectileMovement->MaxSpeed = 1200.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.0f;
	ProjectileMovement->bForceSubStepping = true;
	ProjectileMovement->MaxSimulationTimeStep = 0.016f;
	ProjectileMovement->MaxSimulationIterations = 8;

	VfxComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("VFX"));
	VfxComp->SetupAttachment(RootComponent);
	VfxComp->SetAutoActivate(true);

	// 캡슐 콜리전 추가
	CapsuleCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleCollision"));
	CapsuleCollision->SetupAttachment(RootComponent);

	// 기본 크기
	CapsuleCollision->InitCapsuleSize(25.0f, 180.0f);

	CapsuleCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleCollision->SetCollisionObjectType(ECC_WorldDynamic);

	CapsuleCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	CapsuleCollision->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Ignore);
	CapsuleCollision->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	CapsuleCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	CapsuleCollision->SetNotifyRigidBodyCollision(true);
}

void ABossProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (LifeTime > 0.0f)
	{
		SetLifeSpan(LifeTime);
	}

	// 어떤 콜리전을 쓸지 결정
	ActiveCollision = bUseCapsuleCollision
		? Cast<UPrimitiveComponent>(CapsuleCollision)
		: Cast<UPrimitiveComponent>(Collision);

	if (Collision)			Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (CapsuleCollision)	CapsuleCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (ActiveCollision)
	{
		ActiveCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		ActiveCollision->OnComponentBeginOverlap.AddDynamic(this, &ABossProjectile::OnCollisionBeginOverlap);
		ActiveCollision->OnComponentHit.AddDynamic(this, &ABossProjectile::OnCollisionHit);

		if (AActor* OwnerActor = GetOwner())
		{
			ActiveCollision->IgnoreActorWhenMoving(OwnerActor, true);
		}

		if (ProjectileMovement)
		{
			ProjectileMovement->SetUpdatedComponent(ActiveCollision);
		}
	}

	if (VfxComp && VfxSystem)
	{
		VfxComp->SetAsset(VfxSystem);
		VfxComp->Activate(true);
	}
}

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

void ABossProjectile::OnCollisionBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (bHitOnce) return;

	if (!OtherActor || OtherActor == this) return;

	if (OtherActor == GetOwner()) return;

	HandleImpactAndDestroy(OtherActor);

	if (bDestroyOnPawnHit)
	{
		Destroy();
	}
}

void ABossProjectile::OnCollisionHit(
	UPrimitiveComponent* HitComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit)
{
	if (bHitOnce) return;

	if (!OtherActor || OtherActor == this)
	{
		return;
	}

	if (OtherActor->IsA<APawn>())
	{
		HandleImpactAndDestroy(OtherActor);
		if (bDestroyOnPawnHit) Destroy();
		return;
	}

	if (bDestroyOnWorldHit)
	{
		bHitOnce = true;
		Destroy();
	}
}

void ABossProjectile::HandleImpactAndDestroy(AActor* HitActor)
{
	bHitOnce = true;

	AController* InstigatorController = nullptr;
	if (APawn* InstigatorPawn = GetInstigator())
	{
		InstigatorController = InstigatorPawn->GetController();
	}

	// 데미지 적용
	UGameplayStatics::ApplyDamage(
		HitActor,
		Damage,
		InstigatorController,
		GetOwner() ? GetOwner() : this, // DamageCauser: 보스(있으면) / 아니면 자기
		nullptr
	);


}
