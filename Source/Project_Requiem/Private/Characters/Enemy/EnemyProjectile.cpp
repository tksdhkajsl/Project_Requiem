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

	// 컴포넌트 자체 유도 기능은 끕니다 (우리가 Tick에서 직접 돌릴 예정)
	ProjectileMovement->bIsHomingProjectile = false;

	// 3. 비행 소리 컴포넌트
	FlightAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("FlightAudioComp"));
	FlightAudioComponent->SetupAttachment(RootComponent);
	FlightAudioComponent->bAutoActivate = true; // 생성되자마자 재생

	// 4. 수명 (5초 뒤 자동 삭제)
	InitialLifeSpan = 5.0f;

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

	// 이 로그가 안 뜨면 Tick이 꺼져있는 것임
	//UE_LOG(LogTemp, Warning, TEXT("Homing Tick Running..."));

	// 타겟이 존재하고 유효하다면 추적
	if (TargetActor && TargetActor->IsValidLowLevel())
	{
		// 1. 방향 계산
		FVector Direction = (TargetActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();

		// 2. 부드럽게 회전 (InterpSpeed 20.0f = 강력한 유도)
		FRotator CurrentRot = GetActorRotation();
		FRotator TargetRot = Direction.Rotation();
		FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetRot, DeltaTime, InterpSpeed);

		// 3. 회전 적용
		SetActorRotation(NewRot);

		// 4. 속도 벡터 업데이트 (머리가 향하는 곳으로 날아가게 함)
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


