// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/LastBossCharacter/Projectile/ProjectileBase.h"
#include "Components/ArrowComponent.h"
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

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent(Root)"));
	SetRootComponent(ArrowComponent);

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CapsuleComponent->SetupAttachment(ArrowComponent);
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
	
	// 투사체 액터 활성화
	ActivateProjectile();

	OnActorBeginOverlap.AddDynamic(this, &AProjectileBase::OnPlayerBeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &AProjectileBase::OnPlayerEndOverlap);

	if (!LastBoss.IsValid())
		LastBoss = Cast<ALastBossCharacter>(GetOwner());

	if (LastBoss.IsValid())
		LastBoss->OnLastBossChangedPhase.AddDynamic(this, &AProjectileBase::DeactivateProjectile);

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
	// 이동 함수
	MoveProjectile();

	// bActiveTickDamage가 true면 틱마다 오버랩 중인 플레이어에게 데미지 적용
	if (bActiveTickDamage && OverlappingTargets.Num() > 0)
	{
		// 오버랩 컬렉션을 순회하며 유효한 액터에 대해 데미지 적용
		for (const TWeakObjectPtr<AActor>& WeakActor : OverlappingTargets)
		{
			if (AActor* TargetActor = WeakActor.Get())
			{
				ApplyDamageToPlayer(TargetActor);
			}
		}
	}
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

	// 저장된 오버랩 액터 초기화
	OverlappingTargets.Empty();

	//UE_LOG(LogTemp, Log, TEXT("투사체 지속시간에 의해 스폰된 액터가 삭제되었습니다"));
	Destroy();
}

void AProjectileBase::OnPlayerBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	// 유효성 검사
	if (!bApplyDamageActive || !OtherActor || OtherActor == OverlappedActor)
		return;
	
	// 오버랩된 액터 저장
	OverlappingTargets.Add(OtherActor);

	if (LastBoss.IsValid())
	{
		ApplyDamageToPlayer(OtherActor);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("OnPlayerBeginOverlap 실행실패"));
	}
}

void AProjectileBase::OnPlayerEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	OverlappingTargets.Empty();
}

void AProjectileBase::ApplyDamageToPlayer(AActor* OtherActor)
{
	// 유효성 검사
	if (!OtherActor)
		return;
	APawn* Target = Cast<APawn>(OtherActor);
	if (!Target || !Target->IsPlayerControlled())
		return;

	float AttackDamage = 0.0f;
	if (UStatComponent* BossStat = LastBoss->GetStatComponent())
	{
		// 스폰된 액터의 데미지 계산식
		AttackDamage = BossStat->PhyAtt * Damagemagnification;

		//UE_LOG(LogTemp, Log, TEXT("AttackDamage : %f"), AttackDamage);
	}

	LastBoss->Attack(OtherActor, AttackDamage);

	// SingleHitMode 활성화시 투사체가 데미지를 준 후 비활성화
	if (SingleHitMode)
		DeactivateProjectile();
}

void AProjectileBase::MoveProjectile()
{
	if (TickMoveDistance <= 0.0f)
		return;

	// 투사체 이동 관련
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

