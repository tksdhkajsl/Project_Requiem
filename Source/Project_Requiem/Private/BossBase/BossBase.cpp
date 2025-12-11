// Fill out your copyright notice in the Description page of Project Settings.


#include "Bossbase/BossBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ABossBase::ABossBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = WalkSpeed;
	}
}

// Called when the game starts or when spawned
void ABossBase::BeginPlay()
{
	Super::BeginPlay();

	CurrentHP = MaxHP;

	// 플레이어 캐릭터 캐시
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	TargetCharacter = Cast<ACharacter>(PlayerPawn);

	// 이동 속도 적용
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = WalkSpeed;
	}

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

	// 공격 쿨타임 누적
	TimeSinceLastMeleeAttack += DeltaTime;

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

	EBossState OldState = CurrentState;
	CurrentState = NewState;

	// 상태 변경 이벤트 브로드캐스트
	OnBossStateChanged.Broadcast(NewState, OldState);
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

	case EBossState::Attack:
		UpdateAttack(DeltaTime);
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
	// 등장 모션 , 포즈 유지
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
	ToTarget.Z = 0.0f;	

	const float DistanceToTarget = ToTarget.Length();

	

	// 공격 범위 이하면 공격 상태로 전환
	if (DistanceToTarget <= MeleeAttackRange)
	{
		SetBossState(EBossState::Attack);
		return;
	}

	// 너무 가까우면 멈추기만 하고 chase 유지
	if (DistanceToTarget <= StoppingDistance)
	{
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
	if (!TargetCharacter)
	{
		SetBossState(EBossState::Idle);
		return;
	}

	const FVector BossLocation = GetActorLocation();
	const FVector TargetLocation = TargetCharacter->GetActorLocation();

	FVector ToTarget = TargetLocation - BossLocation;
	ToTarget.Z = 0.0f;

	const float DistanceToTarget = ToTarget.Length();

	// 공격 사거리 밖으로 나가면 다시 쫓기
	if (DistanceToTarget > MeleeAttackRange)
	{
		SetBossState(EBossState::Chase);
		return;
	}

	// 타겟 바라보기
	ToTarget.Normalize();
	const FRotator TargetRot = ToTarget.Rotation();
	const FRotator NewRot = FMath::RInterpTo(GetActorRotation(), TargetRot, DeltaTime, 10.0f);
	SetActorRotation(NewRot);

	// 공격 쿨타임 체크
	if (TimeSinceLastMeleeAttack >= MeleeAttackInterval)
	{
		PerformMeleeAttack();
		TimeSinceLastMeleeAttack = 0.0f;
	}

}

void ABossBase::UpdatePhaseChange(float DeltaTime)
{
}

void ABossBase::UpdateDead(float DeltaTime)
{
}

// 보스 데미지 받기
float ABossBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// 페이즈 확인
	if (bUsePhaseSystem && CurrentPhase == 1 && MaxHP > 0.0f)
	{
		const float HPRatio = CurrentHP / MaxHP;

		if (HPRatio <= Phase2StartHPRatio)
		{
			const int32 OldPhase = CurrentPhase;
			CurrentPhase = 2;

			// 페이즈 변경 델리게이트
			OnBossPhaseChanged.Broadcast(CurrentPhase, OldPhase);

			OnPhaseChanged(CurrentPhase, OldPhase);

			// 상태를 페이즈 체인지로 전환 (연출용)
			SetBossState(EBossState::PhaseChange);
		}
	}

	if (ActualDamage <= 0.0f || CurrentState == EBossState::Dead)
	{
		return 0.0f;
	}

	CurrentHP = FMath::Clamp(CurrentHP - ActualDamage, 0.0f, MaxHP);

	// 브로드캐스트(현재HP, 최대HP, 받은 데미지)
	OnBossDamaged.Broadcast(CurrentHP, MaxHP, ActualDamage);

	if (CurrentHP <= 0.0f)
	{
		SetBossState(EBossState::Dead);

		if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
		{
			MoveComp->StopMovementImmediately();
		}

		// 브로드캐스트 EXP
		OnBossDead.Broadcast(EXP);
	}

	return ActualDamage;

}

// 페이즈 전환 구현
void ABossBase::OnPhaseChanged_Implementation(int32 NewPhase, int32 OldPhase)
{
	// 2페이즈 진입시 스탯 변경
	if (NewPhase == 2 && bUsePhaseSystem)
	{
		// 이동 속도, 공격력 배수 적용
		WalkSpeed *= Phase2_WalkSpeedMultiplier;
		MeleeDamage *= Phase2_MeleeDamageMultiplier;

		if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
		{
			MoveComp->MaxWalkSpeed = WalkSpeed;
		}
	}
}

// 페이즈 전환 끝
void ABossBase::FinishPhaseChange()
{
	if (CurrentState == EBossState::PhaseChange)
	{
		SetBossState(EBossState::Chase);
	}
}

//보스 근접 공격 수행
void ABossBase::PerformMeleeAttack()
{
	if (CurrentState == EBossState::Dead)
	{
		return;
	}

	if (!TargetCharacter)
	{
		return;
	}

	const float Distance = FVector::Dist2D(GetActorLocation(), TargetCharacter->GetActorLocation());
	if (Distance > MeleeAttackRange)
	{
		return;
	}

	// 공격 몽타주 재생
	if (MeleeAttackMontage)
	{
		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			if (!AnimInstance->IsAnyMontagePlaying())
			{
				AnimInstance->Montage_Play(MeleeAttackMontage);
			}
		}
	}

	// 데미지 처리
	AController* BossController = GetController();

	UGameplayStatics::ApplyDamage(
		TargetCharacter,
		MeleeDamage,	// 데미지 양
		BossController,
		this,
		nullptr			// 데미지 타입
	);

}
