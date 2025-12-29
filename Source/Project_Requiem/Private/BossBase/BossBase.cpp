#include "BossBase/BossBase.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"

#include "ComponentSystems/Public/Stats/StatComponent.h"
#include "BossBase/Projectile/BossProjectile.h"

#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"


void ABossBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 쿨타임 누적
	TimeSinceLastMeleeAttack += DeltaTime;
	TimeSinceLastRangedAttack += DeltaTime;

	UpdateState(DeltaTime);

}

void ABossBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

ABossBase::ABossBase()
{
	PrimaryActorTick.bCanEverTick = true;

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = WalkSpeed;
	}

	// 보스 배경음악 오디오 컴포넌트 생성
	BossBGMAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("BossBGMAudioComp"));
	BossBGMAudioComp->SetupAttachment(RootComponent);
	BossBGMAudioComp->bAutoActivate = false;

}

void ABossBase::BeginPlay()
{
	Super::BeginPlay();

	//========================
	// Reset용 초기 상태 저장
	//========================
	InitialLocation = GetActorLocation();
	InitialRotation = GetActorRotation();

	InitialPhase = CurrentPhase;
	bInitialUseRangedAttack = bUseRangedAttack;

	InitialWalkSpeed = WalkSpeed;
	InitialPhyAtt = GetStatComponent()->PhyAtt;

	// 체력 초기화
	GetStatComponent()->ChangeStatCurrent(EFullStats::Health, GetStatComponent()->GetStatMax(EFullStats::Health));

	// 플레이어 캐릭터 캐시
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	TargetCharacter = Cast<ACharacter>(PlayerPawn);

	// 이동 속도 적용
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = WalkSpeed;
	}

	if (bAutoStartChase && TargetCharacter)
	{
		SetBossState(EBossState::Chase);
	}
	else
	{
		SetBossState(EBossState::Idle);
	}
}

void ABossBase::SetBossState(EBossState NewState)
{
	// 같은 상태면 아무것도 안 함
	if (CurrentState == NewState)
	{
		return;
	}

	EBossState OldState = CurrentState;
	CurrentState = NewState;

	switch (CurrentState)
	{
	case EBossState::Dead:
	{
		// 이동 즉시 정지
		if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
		{
			MoveComp->StopMovementImmediately();
		}

		// 캡슐 콜리전 끔
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;

		// 죽음 몽타주 재생
		if (DeathMontage && AnimInstance)
		{
			AnimInstance->Montage_Play(DeathMontage);


			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &ABossBase::OnDeathMontageEnded);
			AnimInstance->Montage_SetEndDelegate(EndDelegate, DeathMontage);
		}
		else
		{
			if (bDestroyOnDeath)
			{
				K2_DestroyActor();
			}
			else
			{
				SetActorHiddenInGame(true);
				SetActorEnableCollision(false);
				SetActorTickEnabled(false);
				GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

				if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
				{
					MoveComp->StopMovementImmediately();
					MoveComp->DisableMovement();
				}

				StopBossBGM();
			}

		}


	}
	break;

	case EBossState::PhaseChange:
	{
		// 페이즈 변경 연출 몽타주 재생
		UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
		if (PhaseChangeMontage && AnimInstance)
		{
			AnimInstance->Montage_Play(PhaseChangeMontage);
		}

		// 연출 중에는 이동 막기
		if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
		{
			MoveComp->StopMovementImmediately();
			MoveComp->DisableMovement();
		}

		// 몽타주가 없으면 즉시 페이즈 전환 종료
		if (!PhaseChangeMontage)
		{
			FinishPhaseChange();
		}
	}
	break;

	default:
		break;
	}

	// 상태 변경 이벤트 브로드캐스트
	OnBossStateChanged.Broadcast(NewState, OldState);
}

void ABossBase::UpdateState(float DeltaTime)
{
	switch (CurrentState)
	{
	case EBossState::Idle:        UpdateIdle(DeltaTime);        break;
	case EBossState::Chase:       UpdateChase(DeltaTime);       break;
	case EBossState::Attack:      UpdateAttack(DeltaTime);      break;
	case EBossState::PhaseChange: UpdatePhaseChange(DeltaTime); break;
	case EBossState::Dead:        UpdateDead(DeltaTime);        break;
	default: break;
	}
}

void ABossBase::UpdateIdle(float DeltaTime)
{
	// 등장 모션, 포즈 유지
}

void ABossBase::UpdateChase(float DeltaTime)
{
	if (bMovementLocked)
	{
		return;
	}

	if (!TargetCharacter)
	{
		SetBossState(EBossState::Idle);
		return;
	}

	// HitReact 중이면 추적을 멈출지 계속할지 선택
	if (bIsInHitReact)
	{
		if (bStopChaseWhileHitReact)
		{
			return;
		}
	}

	// 공격 전환만 막기(추적은 가능)
	const bool bBlockAttackTransition = IsAttackBlocked();

	const FVector BossLocation = GetActorLocation();
	const FVector TargetLocation = TargetCharacter->GetActorLocation();

	FVector ToTarget = TargetLocation - BossLocation;
	ToTarget.Z = 0.0f;

	const float DistanceToTarget = ToTarget.Length();


	if (!bBlockAttackTransition)
	{
		if (DistanceToTarget <= MeleeZoneMaxRange)
		{
			SetBossState(EBossState::Attack);
			return;
		}

		if (bUseRangedAttack && DistanceToTarget >= RangedZoneMinRange)
		{
			SetBossState(EBossState::Attack);
			return;
		}
	}


	if (DistanceToTarget <= StoppingDistance)
	{
		return;
	}

	ToTarget.Normalize();

	// 타겟 바라보게 회전
	const FRotator TargetRot = ToTarget.Rotation();
	const FRotator NewRot = FMath::RInterpTo(GetActorRotation(), TargetRot, DeltaTime, 5.0f);
	SetActorRotation(NewRot);

	AddMovementInput(GetActorForwardVector(), 1.0f);
}

void ABossBase::UpdateAttack(float DeltaTime)
{
	if (!TargetCharacter)
	{
		SetBossState(EBossState::Idle);
		return;
	}

	// HitReact중이면 공격 로직 막기
	if (bIsInHitReact)
	{
		return;
	}

	// HitReact/패턴 후딜(Recovery) 시간 동안 공격 금지
	if (IsAttackBlocked())
	{
		return;
	}

	UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;

	const bool bRangedPlaying =
		(AnimInstance && RangedAttackMontage && AnimInstance->Montage_IsPlaying(RangedAttackMontage));

	if (bRangedPlaying)
	{
		return; // 원거리 몽타주 재생 중엔 공격 상태 로직(전환/패턴선택/이동) 전부 정지
	}

	const FVector BossLocation = GetActorLocation();
	const FVector TargetLocation = TargetCharacter->GetActorLocation();

	FVector ToTarget = TargetLocation - BossLocation;
	ToTarget.Z = 0.0f;

	const float DistanceToTarget = ToTarget.Length();

	// 근접 몽타주 재생중인지 체크

	const bool bMeleePlaying =
		(AnimInstance && MeleeAttackMontage && AnimInstance->Montage_IsPlaying(MeleeAttackMontage));

	if (bMeleePlaying)
	{
		return;
	}

	if (DistanceToTarget > MeleeZoneMaxRange && DistanceToTarget < RangedZoneMinRange)
	{
		//근접 몽타주 재생 중에는 chase로 못넘어가게 막기
		if (!bIsExecutingPattern && !bMeleePlaying)
		{
			UnlockMovement();
			SetBossState(EBossState::Chase);
		}
		return;
	}

	// 타겟 바라보기
	ToTarget.Normalize();
	const FRotator TargetRot = ToTarget.Rotation();
	const FRotator NewRot = FMath::RInterpTo(GetActorRotation(), TargetRot, DeltaTime, 10.0f);
	SetActorRotation(NewRot);

	// 패턴 실행 중이면 기다림(노티파이/몽타주 끝에서 FinishCurrentPattern 호출)
	if (bIsExecutingPattern)
	{
		return;
	}

	// 패턴 선택
	const EBossPattern Next = SelectPattern(DistanceToTarget);

	if (Next == EBossPattern::None)
	{

		if (DistanceToTarget <= MeleeZoneMaxRange)
		{
			if (TimeSinceLastMeleeAttack >= MeleeAttackInterval)
			{
				PerformMeleeAttack();
				TimeSinceLastMeleeAttack = 0.0f;
			}
		}
		else if (bUseRangedAttack && DistanceToTarget >= RangedZoneMinRange)
		{
			if (TimeSinceLastRangedAttack >= RangedAttackInterval)
			{
				PerformRangedAttack();
				TimeSinceLastRangedAttack = 0.0f;
			}
		}
		return;

	}

	// 선택된 패턴 실행
	ExecutePattern(Next);
}

void ABossBase::UpdatePhaseChange(float DeltaTime)
{
}

void ABossBase::UpdateDead(float DeltaTime)
{
}


void ABossBase::ReceiveDamage(float DamageAmount)
{
	//==========================================================
	// (수정) TakeDamage에 있던 조건문들 ReceiveDamage로 이동
	//==========================================================

	if (DamageAmount <= 0.0f || CurrentState == EBossState::Dead)
	{
		return;
	}
	// 무적이면 데미지 무시
	if (bIsInvulnerable)
	{
		return;
	}

	Super::ReceiveDamage(DamageAmount);

	OnBossStatUpdated.Broadcast(GetStatComponent()->GetStatCurrent(EFullStats::Health), GetStatComponent()->GetStatMax(EFullStats::Health), BossName);

	if (GetStatComponent()->GetStatCurrent(EFullStats::Health) <= 0.0f)//GetStatComponent()->GetStatMax(EFullStats::Health)
	{
		SetBossState(EBossState::Dead);

		if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
		{
			MoveComp->StopMovementImmediately();
		}

		// 이벤트 : 경험치 보상
		OnBossDead.Broadcast(EXP);

		return;
	}

	bool bWillEnterPhaseChange = false;

	if (bUsePhaseSystem && CurrentPhase == 1 && GetStatComponent()->GetStatMax(EFullStats::Health) > 0.0f)
	{
		const float HPRatio = GetStatComponent()->GetStatCurrent(EFullStats::Health) / GetStatComponent()->GetStatMax(EFullStats::Health);

		if (HPRatio <= Phase2StartHPRatio)
		{
			bWillEnterPhaseChange = true;
		}
	}

	// 페이즈 넘어갈때는 피격 스킵
	if (!bWillEnterPhaseChange || bAllowHitReactBeforePhaseChange)
	{
		TryPlayHitReact();
	}

	if (bWillEnterPhaseChange)
	{
		const int32 OldPhase = CurrentPhase;
		CurrentPhase = 2;

		// 외부에 페이즈 변경 알림
		OnBossPhaseChanged.Broadcast(CurrentPhase, OldPhase);

		OnPhaseChanged(CurrentPhase, OldPhase);

		SetBossState(EBossState::PhaseChange);

		if (bAutoSwitchBGMOnPhaseChanged)
		{
			SwitchBossBGMByPhase(CurrentPhase);
		}
	}

	return;
}

// 데미지 처리 핵심
float ABossBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	LastHitInstigator = EventInstigator;

	ReceiveDamage(DamageAmount);
	return DamageAmount;
}

void ABossBase::OnPhaseChanged_Implementation(int32 NewPhase, int32 OldPhase)
{
	if (NewPhase == 2 && bUsePhaseSystem)
	{
		WalkSpeed *= Phase2_WalkSpeedMultiplier;
		GetStatComponent()->PhyAtt *= Phase2_MeleeDamageMultiplier;

		if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
		{
			MoveComp->MaxWalkSpeed = WalkSpeed;
		}
	}
}

// 페이즈 전환 끝
void ABossBase::FinishPhaseChange()
{
	if (CurrentState != EBossState::PhaseChange) return;

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->SetMovementMode(MOVE_Walking);
	}

	UnlockMovement();

	SetBossState(EBossState::Chase);
}

// 페이즈 전환 범위 공격 실행
void ABossBase::ApplyPhaseChangeAOE()
{
	if (CurrentState == EBossState::Dead) return;
	if (!GetWorld()) return;

	if (PhaseChangeAoERadius <= 0.0f || PhaseChangeAoEDamage <= 0.0f) return;

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);

	UGameplayStatics::ApplyRadialDamage(
		GetWorld(),
		PhaseChangeAoEDamage,
		GetActorLocation(),
		PhaseChangeAoERadius,
		nullptr,
		IgnoreActors,
		this,
		GetController(),
		true,
		ECC_Visibility
	);
}

void ABossBase::ApplyMeleeDamage()
{
	if (CurrentState == EBossState::Dead) return;
	if (!TargetCharacter) return;

	const float Distance = FVector::Dist2D(GetActorLocation(), TargetCharacter->GetActorLocation());
	if (Distance > MeleeAttackRange) return;

	AController* BossController = GetController();

	UGameplayStatics::ApplyDamage(
		TargetCharacter,
		GetStatComponent()->PhyAtt,
		BossController,
		this,
		nullptr
	);
}

// 기본 원거리 공격
void ABossBase::ApplyRangedAttack()
{
	ApplyRangedAttackFromSocket(RightHandSocketName);
}



// 몽타주 재생만
void ABossBase::PerformMeleeAttack()
{
	if (CurrentState == EBossState::Dead) return;
	if (!TargetCharacter) return;

	const float Distance = FVector::Dist2D(GetActorLocation(), TargetCharacter->GetActorLocation());
	if (Distance > MeleeAttackRange) return;

	LockMovement();

	if (MeleeAttackMontage)
	{
		if (UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr)
		{
			if (!AnimInstance->Montage_IsPlaying(MeleeAttackMontage))
			{
				AnimInstance->Montage_Play(MeleeAttackMontage);

				// 근접 몽타주 끝나면 이동 잠금 해제
				FOnMontageEnded EndDelegate;
				EndDelegate.BindUObject(this, &ABossBase::OnMeleeMontageEnded);
				AnimInstance->Montage_SetEndDelegate(EndDelegate, MeleeAttackMontage);
			}
		}
	}
	else
	{
		UnlockMovement();
	}
}

void ABossBase::ApplyRangedAttackFromSocket(FName SocketName)
{
	if (CurrentState == EBossState::Dead) return;
	if (!bUseRangedAttack || !TargetCharacter) return;

	TSubclassOf<ABossProjectile> UseClass = GetRangedProjectileClassByPhase();
	if (!UseClass) return;

	FVector MuzzleLocation;
	FRotator BaseRot;

	USkeletalMeshComponent* MeshComp = GetMesh();

	if (MeshComp && SocketName != NAME_None && MeshComp->DoesSocketExist(SocketName))
	{
		MuzzleLocation = MeshComp->GetSocketLocation(SocketName);
		BaseRot = (TargetCharacter->GetActorLocation() - MuzzleLocation).Rotation();
	}
	else
	{
		// 소켓이 없으면: 임시 위치(앞쪽+위쪽)에서 발사
		MuzzleLocation = GetActorLocation() + GetActorForwardVector() * 100.0f + FVector(0, 0, 50.0f);
		BaseRot = (TargetCharacter->GetActorLocation() - MuzzleLocation).Rotation();
	}

	const bool bDoMulti =
		(CurrentPhase >= 2) && bPhase2_UseMultiShot && (Phase2_RangedShotCount > 1);

	const int32 ShotCount = bDoMulti ? Phase2_RangedShotCount : 1;

	// 좌우 퍼짐 각도 계산
	const float Spread = bDoMulti ? Phase2_RangedSpreadYaw : 0.0f;
	const float Half = (ShotCount - 1) * 0.5f;

	AController* BossController = GetController();

	for (int32 i = 0; i < ShotCount; ++i)
	{
		const float YawOffset = (i - Half) * Spread;
		const FRotator ShotRot = BaseRot + FRotator(0, YawOffset, 0);

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();

		ABossProjectile* Projectile = GetWorld()->SpawnActor<ABossProjectile>(
			UseClass, MuzzleLocation, ShotRot, SpawnParams);


		if (!Projectile) continue;

		Projectile->InitProjectile(RangedDamage, BossController);
	}
}




// 무적 능력
void ABossBase::StartInvulnerability(float Duration)
{
	if (Duration <= 0.0f) return;
	if (CurrentState == EBossState::Dead) return;

	bIsInvulnerable = true;


	// 이미 무적이면 남은 시간 연장
	GetWorldTimerManager().ClearTimer(InvulnerableTimerHandle);
	GetWorldTimerManager().SetTimer(
		InvulnerableTimerHandle,
		this,
		&ABossBase::EndInvulnerability,
		Duration,
		false
	);
}

void ABossBase::EndInvulnerability()
{
	bIsInvulnerable = false;

}

// 범위 공격
void ABossBase::ApplyCurrentPatternAOE()
{
	if (CurrentState == EBossState::Dead) return;
	if (!GetWorld()) return;

	const float Radius = PendingAoERadius;
	const float Damage = PendingAoEDamage;

	if (Radius <= 0.0f || Damage <= 0.0f) return;

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);

	AController* InstigatorController = GetController();

	UGameplayStatics::ApplyRadialDamage(
		GetWorld(),
		Damage,
		GetActorLocation(),
		Radius,
		nullptr,
		IgnoreActors,
		this,
		InstigatorController,
		true,
		ECC_Visibility
	);
}

void ABossBase::StartCurrentPatternInvulnerability()
{
	StartInvulnerability(PendingInvulnerableDuration);

	UE_LOG(LogTemp, Warning, TEXT("PendingInvulnerableDuration = %f"), PendingInvulnerableDuration);

}

// 원거리 공격 끝나면 이동 풀기
void ABossBase::OnRangedMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == RangedAttackMontage)
	{
		UnlockMovement();
	}
}

// 근접 몽타주 종료
void ABossBase::OnMeleeMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == MeleeAttackMontage)
	{
		UnlockMovement();
	}
}

void ABossBase::OnDeathMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != DeathMontage) return;

	if (bDestroyOnDeath)
	{
		K2_DestroyActor();
		return;
	}

	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->StopMovementImmediately();
		MoveComp->DisableMovement();
	}

	StopBossBGM();

}

void ABossBase::LockMovement()
{
	if (bMovementLocked) return;
	if (CurrentState == EBossState::Dead) return;

	bMovementLocked = true;

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->StopMovementImmediately();
		MoveComp->DisableMovement();
	}
}

void ABossBase::UnlockMovement()
{
	if (!bMovementLocked)return;
	if (CurrentState == EBossState::Dead)return;

	bMovementLocked = false;

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->SetMovementMode(MOVE_Walking);
	}
}

// 몽타주 있으면 몽타주 재생
// 몽타주 없으면 즉시 발사
void ABossBase::PerformRangedAttack()
{
	if (CurrentState == EBossState::Dead) return;
	if (!bUseRangedAttack || !TargetCharacter) return;

	// 몽타주가 없으면 즉발
	if (!RangedAttackMontage)
	{
		LockMovement();
		ApplyRangedAttack();
		UnlockMovement();
		return;
	}

	LockMovement();

	// 몽타주 재생
	if (UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr)
	{
		if (!AnimInstance->Montage_IsPlaying(RangedAttackMontage))
		{
			AnimInstance->Montage_Play(RangedAttackMontage);

			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &ABossBase::OnRangedMontageEnded);
			AnimInstance->Montage_SetEndDelegate(EndDelegate, RangedAttackMontage);
		}
	}
}


// 패턴 함수


const FBossPatternData* ABossBase::FindPatternData(EBossPattern Pattern) const
{
	for (const FBossPatternData& Data : PatternTable)
	{
		if (Data.Pattern == Pattern)
		{
			return &Data;
		}
	}
	return nullptr;
}

bool ABossBase::IsPatternOffCooldown(EBossPattern Pattern, float Now) const
{
	const float* LastTimePtr = LastPatternUseTime.Find(Pattern);
	if (!LastTimePtr) return true;

	const FBossPatternData* Data = FindPatternData(Pattern);
	if (!Data)
	{
		return true;
	}

	return (Now - *LastTimePtr) >= Data->Cooldown;
}

// 패턴 중단 
void ABossBase::AbortCurrentPatternForHitReact()
{
	if (!bIsExecutingPattern) return;

	bIsExecutingPattern = false;
	CurrentPattern = EBossPattern::None;

	PendingAoERadius = 0.0f;
	PendingAoEDamage = 0.0f;
	PendingInvulnerableDuration = 0.0f;
}

EBossPattern ABossBase::SelectPattern(float DistanceToTarget) const
{
	if (PatternTable.Num() <= 0)
	{
		return EBossPattern::None;
	}

	const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;

	// 후보 수집
	TArray<const FBossPatternData*> Candidates;
	float TotalWeight = 0.0f;

	for (const FBossPatternData& Data : PatternTable)
	{
		if (Data.Pattern == EBossPattern::None) continue;

		// 페이즈 조건
		if (Data.OnlyPhase != 0 && Data.OnlyPhase != CurrentPhase) continue;

		// 거리 조건
		if (DistanceToTarget < Data.MinRange || DistanceToTarget > Data.MaxRange) continue;

		// 쿨타임
		const float* LastTimePtr = LastPatternUseTime.Find(Data.Pattern);
		if (LastTimePtr)
		{
			if ((Now - *LastTimePtr) < Data.Cooldown) continue;
		}

		// 가중치
		if (Data.Weight <= 0.0f) continue;

		Candidates.Add(&Data);
		TotalWeight += Data.Weight;
	}

	if (Candidates.Num() <= 0 || TotalWeight <= 0.0f)
	{
		return EBossPattern::None;
	}

	// 가중치 룰렛
	const float Roll = FMath::FRandRange(0.0f, TotalWeight);
	float Acc = 0.0f;

	for (const FBossPatternData* Data : Candidates)
	{
		Acc += Data->Weight;
		if (Roll <= Acc)
		{
			return Data->Pattern;
		}
	}

	return Candidates.Last()->Pattern;
}

// 실행 패턴
void ABossBase::ExecutePattern(EBossPattern Pattern)
{
	if (Pattern == EBossPattern::None) return;
	if (CurrentState == EBossState::Dead) return;

	const FBossPatternData* Data = FindPatternData(Pattern);
	if (!Data) return;

	PendingAoERadius = Data->AoERadius;
	PendingAoEDamage = Data->AoEDamage;
	PendingInvulnerableDuration = Data->InvulnerableDuration;

	PendingRecoveryTime = DefaultPatternRecoveryTime;

	bIsExecutingPattern = true;
	CurrentPattern = Pattern;

	const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
	LastPatternUseTime.FindOrAdd(Pattern) = Now;

	//이동 중지
	LockMovement();

	// 몽타주 있으면 재생, 실제 패턴은 노티파이에서 처리
	if (Data->Montage)
	{
		UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;

		// 몽타주 기반 패턴인데 AnimInstance가 없으면 실행 불가 → 안전 종료
		if (!AnimInstance)
		{
			FinishCurrentPattern();
			return;
		}

		if (!AnimInstance->Montage_IsPlaying(Data->Montage))
		{
			AnimInstance->Montage_Play(Data->Montage);
		}
		return; // 몽타주 기반 패턴은 노티파이가 끝냄
	}
	switch (Pattern)
	{
	case EBossPattern::Pattern1:
	case EBossPattern::Pattern2:
	case EBossPattern::Pattern3:
	case EBossPattern::Pattern4:
	case EBossPattern::Pattern5:
		// 페이즈2 전용
	case EBossPattern::Pattern9:
	case EBossPattern::Pattern10:
	case EBossPattern::Pattern11:
	case EBossPattern::Pattern12:
	case EBossPattern::Pattern13:
		PerformMeleeAttack();
		break;

	case EBossPattern::Pattern6:
	case EBossPattern::Pattern7:
		// 페이즈2 전용
	case EBossPattern::Pattern14:
	case EBossPattern::Pattern15:
		ApplyRangedAttackFromSocket(RightHandSocketName);
		break;

	case EBossPattern::Pattern8:
		// 페이즈2 전용
	case EBossPattern::Pattern16:
		ApplyRangedAttackFromSocket(RightHandSocketName);
		ApplyRangedAttackFromSocket(LeftHandSocketName);
		break;

	default:
		break;
	}

	FinishCurrentPattern();
}



void ABossBase::TryPlayHitReact()
{
	if (!bEnableHitReact) return;
	if (CurrentState == EBossState::Dead) return;
	if (CurrentState == EBossState::PhaseChange) return;
	if (!HitReactMontage) return;
	if (!bHitReactWhileExecutingPattern && bIsExecutingPattern) return;

	const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
	if ((Now - LastHitReactTime) < HitReactCooldown) return;

	UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
	if (!AnimInstance) return;

	if (AnimInstance->Montage_IsPlaying(HitReactMontage)) return;

	if (bInterruptPatternOnHitReact)
	{
		// 패턴 플래그 강제 중단
		AbortCurrentPatternForHitReact();

		if (AnimInstance->IsAnyMontagePlaying())
		{
			AnimInstance->StopAllMontages(HitReactInterruptBlendOut);
		}

	}

	bIsInHitReact = true;

	AttackBlockedUntilTime = FMath::Max(AttackBlockedUntilTime, Now + HitReactRecoveryTime);


	LockMovement();

	AnimInstance->Montage_Play(HitReactMontage);

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &ABossBase::OnHitReactMontageEnded);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, HitReactMontage);

	LastHitReactTime = Now;
}



bool ABossBase::IsAttackBlocked() const
{
	if (!GetWorld()) return false;
	const float Now = GetWorld()->GetTimeSeconds();
	return Now < AttackBlockedUntilTime;
}

void ABossBase::FinishCurrentPattern()
{
	UnlockMovement();

	bIsExecutingPattern = false;
	CurrentPattern = EBossPattern::None;

	if (GetWorld())
	{
		const float Now = GetWorld()->GetTimeSeconds();
		const float Recovery = FMath::Max(0.0f, PendingRecoveryTime);
		AttackBlockedUntilTime = FMath::Max(AttackBlockedUntilTime, Now + Recovery);
	}

	PendingAoERadius = 0.0f;
	PendingAoEDamage = 0.0f;
	PendingInvulnerableDuration = 0.0f;
	PendingRecoveryTime = 0.0f;

	if (CurrentState == EBossState::Attack)
	{
		SetBossState(EBossState::Chase);
	}
}

void ABossBase::OnHitReactMontageEnded(UAnimMontage* Montage, bool Interrupted)
{
	if (Montage != HitReactMontage) return;
	if (CurrentState == EBossState::Dead) return;
	if (CurrentState == EBossState::PhaseChange) return;

	bIsInHitReact = false;

	if (GetWorld())
	{
		const float Now = GetWorld()->GetTimeSeconds();
		AttackBlockedUntilTime = FMath::Max(AttackBlockedUntilTime, Now + HitReactRecoveryTime);
	}


	UnlockMovement();

	SetBossState(EBossState::Chase);
}

TSubclassOf<ABossProjectile> ABossBase::GetRangedProjectileClassByPhase() const
{
	// 2페이즈면 Phase2 클래스 우선
	if (CurrentPhase >= 2 && RangedProjectileClass_Phase2)
	{
		return RangedProjectileClass_Phase2;
	}

	// 1페이즈면 Phase1 클래스 우선
	if (RangedProjectileClass_Phase1)
	{
		return RangedProjectileClass_Phase1;
	}

	return RangedProjectileClass;
}


// 보스 배경음악
void ABossBase::SetBossBGMSound(USoundBase* NewSound)
{
	BossBGMSound = NewSound;

	if (BossBGMAudioComp && bBossBGMPlaying)
	{
		BossBGMAudioComp->SetSound(BossBGMSound);
		BossBGMAudioComp->Play();
	}
}

void ABossBase::StartBossBGM()
{
	if (!BossBGMAudioComp) return;

	USoundBase* Desired = GetBossBGMSoundByPhase(CurrentPhase);
	if (!Desired) return;

	// 중복 재생 방지
	if (bBossBGMPlaying && BossBGMAudioComp->IsPlaying() && BossBGMAudioComp->Sound == Desired)
	{
		return;
	}

	BossBGMAudioComp->SetSound(Desired);
	BossBGMAudioComp->Play();
	bBossBGMPlaying = true;
}


void ABossBase::StopBossBGM()
{
	if (!BossBGMAudioComp) return;

	BossBGMAudioComp->Stop();
	bBossBGMPlaying = false;
}

// 페이즈 사운드 선택
USoundBase* ABossBase::GetBossBGMSoundByPhase(int32 Phase) const
{
	if (Phase >= 2 && BossBGM_Phase2) return BossBGM_Phase2;
	if (BossBGM_Phase1) return BossBGM_Phase1;

	return BossBGMSound;
}

void ABossBase::SwitchBossBGMByPhase(int32 Phase)
{
	if (!BossBGMAudioComp) return;

	USoundBase* Desired = GetBossBGMSoundByPhase(Phase);
	if (!Desired) return;

	if (BossBGMAudioComp->Sound == Desired && BossBGMAudioComp->IsPlaying())
	{
		bBossBGMPlaying = true;
		return;
	}

	if (BossBGMAudioComp->IsPlaying() && BGMFadeOutTime > 0.0f)
	{
		BossBGMAudioComp->FadeOut(BGMFadeOutTime, 0.0f);
	}
	else
	{
		BossBGMAudioComp->Stop();
	}

	BossBGMAudioComp->SetSound(Desired);

	if (BGMFadeInTime > 0.0f)
	{
		BossBGMAudioComp->FadeIn(BGMFadeInTime, 1.0f);
	}
	else
	{
		BossBGMAudioComp->Play();
	}

	bBossBGMPlaying = true;
}

void ABossBase::ActivateBossBattle()
{
	// 혹시 비활성화되어 있었다면 보스전 시작 시 다시 활성화
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// 플레이어 캐시
	if (!TargetCharacter)
	{
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		TargetCharacter = Cast<ACharacter>(PlayerPawn);
	}

	UnlockMovement();
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->SetMovementMode(MOVE_Walking);
		MoveComp->MaxWalkSpeed = WalkSpeed;
	}

	if (GetStatComponent())
	{
		GetStatComponent()->ChangeStatCurrent(EFullStats::Health, GetStatComponent()->GetStatMax(EFullStats::Health));
	}

	OnBossStatUpdated.Broadcast(GetStatComponent()->GetStatCurrent(EFullStats::Health), GetStatComponent()->GetStatMax(EFullStats::Health), BossName);

	StartBossBGM();

	CurrentState = EBossState::Idle;

	if (TargetCharacter)
	{
		SetBossState(EBossState::Chase);
	}
	else
	{
		SetBossState(EBossState::Idle);
	}

}

void ABossBase::ResetBossToDefault()
{
	// 죽어서 비활성화된 상태였으면 다시 활성화
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	WalkSpeed = InitialWalkSpeed;
	GetStatComponent()->PhyAtt = InitialPhyAtt;

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = WalkSpeed;
	}

	GetWorldTimerManager().ClearTimer(InvulnerableTimerHandle);
	bIsInvulnerable = false;

	bIsExecutingPattern = false;
	CurrentPattern = EBossPattern::None;
	bIsInHitReact = false;

	PendingAoERadius = 0.0f;
	PendingAoEDamage = 0.0f;
	PendingInvulnerableDuration = 0.0f;
	PendingRecoveryTime = 0.0f;

	AttackBlockedUntilTime = 0.0f;
	LastHitReactTime = -9999.0f;

	if (UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr)
	{
		AnimInstance->StopAllMontages(0.1f);
	}

	UnlockMovement();

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->StopMovementImmediately();
		MoveComp->SetMovementMode(MOVE_Walking);
		MoveComp->MaxWalkSpeed = WalkSpeed;
	}

	CurrentPhase = InitialPhase;
	bUseRangedAttack = bInitialUseRangedAttack;

	SetActorLocation(InitialLocation);
	SetActorRotation(InitialRotation);

	if (GetStatComponent())
	{
		GetStatComponent()->ChangeStatCurrent(EFullStats::Health, GetStatComponent()->GetStatMax(EFullStats::Health));

	}

	OnBossStatUpdated.Broadcast(GetStatComponent()->GetStatCurrent(EFullStats::Health), GetStatComponent()->GetStatMax(EFullStats::Health), BossName);

	SetBossState(EBossState::Idle);

	StopBossBGM();

}


