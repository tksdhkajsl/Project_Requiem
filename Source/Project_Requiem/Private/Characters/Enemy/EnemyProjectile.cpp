// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Enemy/EnemyProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Characters/Player/Character/PlayerCharacter.h"

// Sets default values
AEnemyProjectile::AEnemyProjectile()
{
	PrimaryActorTick.bCanEverTick = true; // [필수] 직접 유도를 위해 Tick 켜기

	// 1. 충돌체 설정
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(15.0f);
	CollisionComp->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	CollisionComp->OnComponentHit.AddDynamic(this, &AEnemyProjectile::OnHit);
	RootComponent = CollisionComp;

	// 2. 무브먼트 설정
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 800.f; // 속도를 조금 낮춰서 유도가 잘 되게 함 (800~1000 추천)
	ProjectileMovement->MaxSpeed = 800.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;

	ProjectileMovement->ProjectileGravityScale = 0.0f;
	// 컴포넌트 자체 유도 기능은 끕니다 (우리가 Tick에서 직접 돌릴 예정)
	ProjectileMovement->bIsHomingProjectile = false;

	// 3. 비행 소리 컴포넌트
	FlightAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("FlightAudioComp"));
	FlightAudioComponent->SetupAttachment(RootComponent);
	FlightAudioComponent->bAutoActivate = true; // 생성되자마자 재생	

	MaxLifeTime = 3.5f;
	InitialLifeSpan = MaxLifeTime;
}

// Called when the game starts or when spawned
void AEnemyProjectile::BeginPlay()
{
	Super::BeginPlay();

	// [핵심 해결책] 나를 쏜 사람(Instigator)과는 부딪히지 않게 설정
	if (GetInstigator())
	{
		CollisionComp->IgnoreActorWhenMoving(GetInstigator(), true);

		// 혹시 모르니 이동 무시 리스트에도 추가
		CollisionComp->MoveIgnoreActors.Add(GetInstigator());
	}
}

void AEnemyProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 타겟이 유효할 때만 유도 로직 수행
	if (TargetActor && TargetActor->IsValidLowLevel())
	{
		FVector ToTarget = TargetActor->GetActorLocation() - GetActorLocation();
		float Distance = ToTarget.Size();

		// 내적 계산 (1.0: 정면, 0.0: 옆, -1.0: 뒤)
		float DotValue = FVector::DotProduct(GetActorForwardVector(), ToTarget.GetSafeNormal());

		// [살아있는 시간 계산]
		// InitialLifeSpan에서 남은 수명(GetLifeSpan)을 빼면 '태어난 지 얼마나 됐는지' 알 수 있음
		float TimeAlive = InitialLifeSpan - GetLifeSpan();

		// =================================================================
		// [최종 해결책] 유예 시간 + 엄격한 각도
		// =================================================================
		// 1. 발사 후 0.4초가 지났는가? (초반에는 각도가 안 좋아도 무조건 유도)
		if (TimeAlive > 0.4f)
		{
			// 2. 이제는 엄격하게 검사!
			// 내 정면(0.1f, 약 85도) 범위를 벗어나면, 즉시 유도를 포기하고 직선 비행
			// 거리도 200 정도로 넉넉하게 둬서, 부딪히기 전에 피할 틈을 줌
			if (DotValue < 0.1f || Distance < 40.0f)
			{
				TargetActor = nullptr; // 유도 끝 (직선 비행 시작)
				return;
			}
		}

		// 3. 회전 로직 (유도 계속)
		FVector Direction = ToTarget.GetSafeNormal();
		FRotator CurrentRot = GetActorRotation();
		FRotator TargetRot = Direction.Rotation();

		//회전각 뒤의 float 설정이 클 수록 각 꺽임 강도 높아짐.
		FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetRot, DeltaTime, 5.0f);

		SetActorRotation(NewRot);

		if (ProjectileMovement)
		{
			ProjectileMovement->Velocity = NewRot.Vector() * ProjectileMovement->InitialSpeed;
		}
	}
}

// [핵심] 유도 시작 함수
void AEnemyProjectile::SetHomingTarget(AActor* Target)
{
	// 타겟을 변수에 저장만 해둠
	if (Target)
	{
		TargetActor = Target;
		//UE_LOG(LogTemp, Warning, TEXT("Manual Homing Start! Target: %s"), *Target->GetName());
	}
}

void AEnemyProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//UE_LOG(LogTemp, Log, TEXT("OtherActor: %s "), *OtherActor->GetActorLabel());
	// 나 자신, 나를 쏜 적 무시
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherActor != GetInstigator()))
	{
		// 1. 타격음 재생 (Destroy 되어도 소리는 남음)
		if (ImpactSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
		}

		// 2. 플레이어 데미지 처리
		if (APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor))
		{
			UGameplayStatics::ApplyDamage(Player, Damage, GetInstigatorController(), this, UDamageType::StaticClass());
			Destroy(); // 명중 시 삭제
		}
		else
		{
			Destroy(); // 벽/바닥 등 장애물에 맞아도 삭제
		}
	}
}


