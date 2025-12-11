// Fill out your copyright notice in the Description page of Project Settings.


#include "Bossbase/BossBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ABossBase::ABossBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

// Called when the game starts or when spawned
void ABossBase::BeginPlay()
{
	Super::BeginPlay();

	// 플레이어 캐릭터 캐시
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	TargetCharacter = Cast<ACharacter>(PlayerPawn);

	if (!TargetCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("BossBase: TargetCharacter not found!"));
	}

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	// 전투 시작과 동시에 바로 추적 시작
	if (bAutoStartChase && TargetCharacter)
	{
		SetBossState(EBossState::Chase);
	}
	else
	{
		SetBossState(EBossState::Idle);
	}

}

// Called every frame
void ABossBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateState(DeltaTime);

}

// Called to bind functionality to input
void ABossBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
void ABossBase::SetBossState(EBossState NewState)
{
	if (CurrentState == NewState)
	{
		return;
	}

	CurrentState = NewState;
}

void ABossBase::UpdateState(float DeltaTime)
{
	switch (CurrentState)
	{
	case EBossState::Idle:
		UpdateIdle(DeltaTime);
		break;

	case EBossState::Chase:
		UpdateChase(DeltaTime);
		break;

	case EBossState::PhaseChange:
		UpdatePhaseChange(DeltaTime);
		break;

	case EBossState::Dead:
		UpdateDead(DeltaTime);
		break;

	default:
		break;
	}
}

void ABossBase::UpdateIdle(float DeltaTime)
{
	// 등장 모션 , 포즈 유지 등
}

void ABossBase::UpdateChase(float DeltaTime)
{
	if (!TargetCharacter)
	{
		SetBossState(EBossState::Idle);
		return;
	}

	const FVector BossLocation = GetActorLocation();
	const FVector TargetLocation = TargetCharacter->GetActorLocation();

	FVector ToTarget = TargetLocation - BossLocation;
	ToTarget.Z = 0.0f;	//	평면 기준

	const float DistanceToTarget = ToTarget.Length();

	// 너무 가까우면 멈춤
	if (DistanceToTarget <= StoppingDistance)
	{
		// 여기서 나중에 공격으로 전환
		return;
	}

	ToTarget.Normalize();

	// 타겟을 바라보게 회전
	const FRotator TargetRot = ToTarget.Rotation();
	const FRotator NewRot = FMath::RInterpTo(GetActorRotation(), TargetRot, DeltaTime, 5.0f);
	SetActorRotation(NewRot);

	//전진
	AddMovementInput(GetActorForwardVector(), 1.0f);


}

void ABossBase::UpdateAttack(float DeltaTime)
{
}

void ABossBase::UpdatePhaseChange(float DeltaTime)
{
}

void ABossBase::UpdateDead(float DeltaTime)
{
}


