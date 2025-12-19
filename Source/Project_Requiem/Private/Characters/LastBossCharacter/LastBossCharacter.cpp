// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/LastBossCharacter/LastBossCharacter.h"
#include "ComponentSystems/Public/Stats/StatComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"

ALastBossCharacter::ALastBossCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	SpawnProjectileLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Projectile"));
	SpawnProjectileLocation->SetupAttachment(RootComponent);
}

void ALastBossCharacter::BeginPlay()
{
	Super::BeginPlay();

	LastBossSpawn();
}

void ALastBossCharacter::LastBossSpawn()
{
	OnLastBossName.Broadcast(BossName);

	AddPatternMontage();
	
	if (SpawnMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		// 스폰시 몽타주 실행
		float Duration = GetMesh()->GetAnimInstance()->Montage_Play(SpawnMontage);
		// 몽타주 종료 이후 AI컨트롤러 활성화를 위한 델리게이트 전송
		if (Duration > 0.0f)
		{
			FOnMontageEnded OnMontageEnded;
			OnMontageEnded.BindUObject(this, &ALastBossCharacter::LastBossEndSpawn);
			GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(OnMontageEnded, SpawnMontage);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnMontage가 없습니다"));
	}
}

void ALastBossCharacter::LastBossEndSpawn(UAnimMontage* Montage, bool bInterrupted)
{
	bLastBossInvincible = true;
	OnLastBossSpawn.Broadcast();
}

void ALastBossCharacter::ReceiveDamage(float DamageAmount)
{
	if (!bLastBossInvincible)
		return;

	GetStatComponent()->CurrHP -= DamageAmount;

	OnApplyDamage.Broadcast(DamageAmount);
	Super::ReceiveDamage(DamageAmount);

	if (GetStatComponent() && GetStatComponent()->CurrHP < 0.0f)
		Die();
}

float ALastBossCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	//if (bIsDead) return 0.f;

	LastHitInstigator = EventInstigator;

	ReceiveDamage(DamageAmount);
	return DamageAmount;
}

void ALastBossCharacter::Die()
{
	Super::Die();

	if (Phase == 1)
	{
		// 페이즈 증가
		Phase++;
		// 페이즈 넘어갈 때 체력 다시 회복
		StatComponent->CurrHP = StatComponent->MaxHP;
		// 저장된 페이즈2 스켈레탈 메시로 변경
		GetMesh()->SetSkeletalMesh(PhaseTwoSkeletalMesh);
		// 페이즈 변경 저장
		bPhaseChanged = true;

		// 페이즈 변경 델리게이트 
		OnLastBossChangedPhase.Broadcast();
	}
	else if (Phase == 2)
	{
		StopAnimMontage();

		// 사망 몽타주 출력
		GetMesh()->GetAnimInstance()->Montage_Play(DieMontage, 0.2f);

		// 경험치 드랍
		ApplyExp(DropExp);

		// 보스 사망 델리게이트
		OnLastBossDead.Broadcast();

		Destroy();
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

