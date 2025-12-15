// BossProjectile.cpp
#include "BossBase/Projectile/BossProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

ABossProjectile::ABossProjectile()
{
	PrimaryActorTick.bCanEverTick = false; // 투사체는 Tick 안 써도 충분

	// 1) 콜리전 생성 + 루트 지정
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SetRootComponent(Collision);

	// 구체 반지름
	Collision->InitSphereRadius(12.0f);

	// 2) 콜리전 세팅
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Collision->SetCollisionObjectType(ECC_WorldDynamic);

	// 기본은 전부 무시
	Collision->SetCollisionResponseToAllChannels(ECR_Ignore);

	// 월드(벽/바닥)에는 Block
	Collision->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	Collision->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);

	// Pawn(플레이어/적)은 Overlap로 감지(“맞았는지” 판정)
	Collision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	// Hit 이벤트 발생하도록 설정(블록 충돌용)
	Collision->SetNotifyRigidBodyCollision(true);

	// 3) ProjectileMovement 생성 + 기본 파라미터
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = Collision;
	ProjectileMovement->InitialSpeed = 1200.0f;
	ProjectileMovement->MaxSpeed = 1200.0f;
	ProjectileMovement->bRotationFollowsVelocity = true; // 진행 방향으로 회전
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.0f;   // 중력 없음(직선탄)
}

void ABossProjectile::BeginPlay()
{
	Super::BeginPlay();

	// 수명 설정(시간 지나면 자동 파괴)
	if (LifeTime > 0.0f)
	{
		SetLifeSpan(LifeTime);
	}

	// 이벤트 바인딩(충돌 콜백 연결)
	if (Collision)
	{
		Collision->OnComponentBeginOverlap.AddDynamic(this, &ABossProjectile::OnCollisionBeginOverlap);
		Collision->OnComponentHit.AddDynamic(this, &ABossProjectile::OnCollisionHit);
	}

	// Owner(보스) 무시(자기한테 맞지 않게)
	if (AActor* OwnerActor = GetOwner())
	{
		Collision->IgnoreActorWhenMoving(OwnerActor, true);
	}
}

void ABossProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Tick을 꺼놨으니 일반적으로 실행 안 됨
}

void ABossProjectile::InitProjectile(float InDamage, AController* InInstigatorController)
{
	Damage = InDamage; // 보스가 전달한 데미지 반영

	// 인스티게이터(가해자) 설정: 컨트롤러의 Pawn을 instigator로
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
	// 이미 1회 히트 처리했으면 무시
	if (bHitOnce) return;

	// 유효하지 않은 액터면 무시
	if (!OtherActor || OtherActor == this) return;

	// Owner(보스)면 무시
	if (OtherActor == GetOwner()) return;

	// Pawn(플레이어) 등과 Overlap 시 “맞췄다” 처리
	HandleImpactAndDestroy(OtherActor);
}

void ABossProjectile::OnCollisionHit(
	UPrimitiveComponent* HitComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit)
{
	if (bHitOnce) return;

	// Hit인데 OtherActor가 없거나 자기 자신이면 그냥 파괴(벽/바닥 같은 케이스 대비)
	if (!OtherActor || OtherActor == this)
	{
		bHitOnce = true;
		Destroy();
		return;
	}

	// Owner면 무시
	if (OtherActor == GetOwner()) return;

	// 월드에 박히든, 다른 액터에 박히든 공통 처리
	HandleImpactAndDestroy(OtherActor);
}

void ABossProjectile::HandleImpactAndDestroy(AActor* HitActor)
{
	bHitOnce = true;

	// InstigatorController 구하기(ApplyDamage에 넣어주면 “누가 때렸는지” 판정에 도움)
	AController* InstigatorController = nullptr;
	if (APawn* InstigatorPawn = GetInstigator())
	{
		InstigatorController = InstigatorPawn->GetController();
	}

	// 데미지 적용(피격 액터에게)
	UGameplayStatics::ApplyDamage(
		HitActor,
		Damage,
		InstigatorController,
		GetOwner() ? GetOwner() : this, // DamageCauser: 보스(있으면) / 아니면 자기
		nullptr
	);

	// 투사체 파괴
	Destroy();
}
