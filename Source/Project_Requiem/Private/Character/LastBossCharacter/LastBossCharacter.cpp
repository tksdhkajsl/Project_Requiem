// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/LastBossCharacter/LastBossCharacter.h"
#include "ComponentSystems/Public/Stats/StatComponent.h"
#include "Components/SceneComponent.h"

ALastBossCharacter::ALastBossCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	SpawnProjectileLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Projectile"));
	SpawnProjectileLocation->SetupAttachment(RootComponent);
}

void ALastBossCharacter::BeginPlay()
{
	Super::BeginPlay();

	OnLastBossName.Broadcast(BossName);

	AddPatternMontage();
	PlayAnimMontage(SpawnMontage);
}

void ALastBossCharacter::Move(const FVector& Direction, float Value)
{
	Super::Move(Direction, Value);

	Move(Direction, Value);
}

void ALastBossCharacter::ApplyDamage(float DamageAmount)
{
	OnApplyDamage.Broadcast(DamageAmount);
}

float ALastBossCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void ALastBossCharacter::Die()
{
	if (Phase == 1)
	{
		Phase++;
		StatComponent->CurrHP = StatComponent->MaxHP;
	}
	else if (Phase == 2)
	{
		PlayAnimMontage(DieMontage);
		ApplyExp(DropExp);
	}
}

void ALastBossCharacter::ApplyExp(float ExpAmount)
{
	OnApplyExp.Broadcast(ExpAmount);
}

void ALastBossCharacter::AddPatternMontage()
{
	// 보스 페이즈 1 패턴 초기화 및 추가
	PhaseOnePatterns.Empty();
	PhaseOnePatterns.Add(PhaseOneMontage1);
	PhaseOnePatterns.Add(PhaseOneMontage2);
	PhaseOnePatterns.Add(PhaseOneMontage3);
	PhaseOnePatterns.Add(PhaseOneMontage4);
	PhaseOnePatterns.Add(PhaseOneMontage5);

	// 보스 페이즈 2 패턴 초기화 및 추가
	PhaseTwoPatterns.Empty();
	PhaseTwoPatterns.Add(PhaseTwoMontage1);
	PhaseTwoPatterns.Add(PhaseTwoMontage2);
	PhaseTwoPatterns.Add(PhaseTwoMontage3);
	PhaseTwoPatterns.Add(PhaseTwoMontage4);
	PhaseTwoPatterns.Add(PhaseTwoMontage5);
}

void ALastBossCharacter::RandomPatternPlay()
{
	int32 Index = FMath::RandRange(1, PhaseOnePatterns.Num());
	if (Phase == 1)			
	{
		if (PhaseOnePatterns.Num() == 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("PhaseOnePatterns is Empty!!!"));
			return;
		}

		// 페이즈 1 몽타주 출력
		if (PhaseOnePatterns.IsValidIndex(Index) && PhaseOnePatterns[Index])
		{
			PlayAnimMontage(PhaseOnePatterns[Index]);
		}
	}
	else if (Phase == 2)	
	{
		if (PhaseOnePatterns.Num() == 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("PhaseTwoPatterns is Empty!!!"));
			return;
		}

		// 페이즈 2 몽타주 출력
		if (PhaseTwoPatterns.IsValidIndex(Index) && PhaseTwoPatterns[Index])
		{
			PlayAnimMontage(PhaseTwoPatterns[Index]);
		}
	}	
}
