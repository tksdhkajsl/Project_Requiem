#include "BossBase/BossBase.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "BossBase/Projectile/BossProjectile.h"


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
}

void ABossBase::BeginPlay()
{
	Super::BeginPlay();

	// 체력 초기화
	CurrentHP = MaxHP;

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

		// 죽음 몽타주 재생
		if (DeathMontage)
		{
			if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
			{
				AnimInstance->Montage_Play(DeathMontage);
			}
		}

		// 3.3초 뒤 Destroy
		FTimerHandle DestroyTimerHandle;
		GetWorldTimerManager().SetTimer(
			DestroyTimerHandle,
			this,
			&ABossBase::K2_DestroyActor,
			3.3f, false
		);
	}
	break;

	case EBossState::PhaseChange:
	{
		// 페이즈 변경 연출 몽타주 재생
		if (PhaseChangeMontage)
		{
			if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
			{
				AnimInstance->Montage_Play(PhaseChangeMontage);
			}
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

	// 공격 진입 거리
	const float AttackEnterRange = bUseRangedAttack ? RangedAttackRange : MeleeAttackRange;

	if (DistanceToTarget <= AttackEnterRange)
	{
		SetBossState(EBossState::Attack);
		return;
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

	const FVector BossLocation = GetActorLocation();
	const FVector TargetLocation = TargetCharacter->GetActorLocation();

	FVector ToTarget = TargetLocation - BossLocation;
	ToTarget.Z = 0.0f;

	const float DistanceToTarget = ToTarget.Length();

	const float AttackMaxRange = bUseRangedAttack ? RangedAttackRange : MeleeAttackRange;

	if (DistanceToTarget > AttackMaxRange)
	{
		if (!bIsExecutingPattern)
		{
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

		if (DistanceToTarget <= MeleeAttackRange)
		{
			if (TimeSinceLastMeleeAttack >= MeleeAttackInterval)
			{
				PerformMeleeAttack();         
				TimeSinceLastMeleeAttack = 0.0f;
			}
		}
		else if (bUseRangedAttack && DistanceToTarget <= RangedAttackRange)
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


// 데미지 처리 핵심
float ABossBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (ActualDamage <= 0.0f || CurrentState == EBossState::Dead)
	{
		return 0.0f;
	}

	// 무적이면 데미지 무시
	if (bIsInvulnerable)
	{
		return 0.0f;
	}

	CurrentHP = FMath::Clamp(CurrentHP - ActualDamage, 0.0f, MaxHP);

	OnBossDamaged.Broadcast(CurrentHP, MaxHP);

	if (CurrentHP <= 0.0f)
	{
		SetBossState(EBossState::Dead);

		if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
		{
			MoveComp->StopMovementImmediately();
		}

		// 사망 보상
		OnBossDead.Broadcast(EXP);

		return ActualDamage;
	}

	bool bWillEnterPhaseChange = false;

	if (bUsePhaseSystem && CurrentPhase == 1 && MaxHP > 0.0f)
	{
		const float HPRatio = CurrentHP / MaxHP;

		if (HPRatio <= Phase2StartHPRatio)
		{
			bWillEnterPhaseChange = true;
		}
	}

			

	// 페이즈 넘어갈때는 피격 스킵
	if (!bWillEnterPhaseChange)
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
	}

	return ActualDamage;
}

void ABossBase::OnPhaseChanged_Implementation(int32 NewPhase, int32 OldPhase)
{
	if (NewPhase == 2 && bUsePhaseSystem)
	{
		WalkSpeed *= Phase2_WalkSpeedMultiplier;   
		MeleeDamage *= Phase2_MeleeDamageMultiplier; 

		if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
		{
			MoveComp->MaxWalkSpeed = WalkSpeed;
		}
	}
}

void ABossBase::FinishPhaseChange()
{
	if (CurrentState == EBossState::PhaseChange)
	{
		if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
		{
			MoveComp->SetMovementMode(MOVE_Walking);
		}

		SetBossState(EBossState::Chase);
	}
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
		MeleeDamage,
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

	GetCharacterMovement()->StopMovementImmediately();

	if (MeleeAttackMontage)
	{
		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			if (!AnimInstance->Montage_IsPlaying(MeleeAttackMontage))
			{
				AnimInstance->Montage_Play(MeleeAttackMontage);
			}
		}
	}
}

void ABossBase::ApplyRangedAttackFromSocket(FName SocketName)
{
	if (CurrentState == EBossState::Dead) return;
	if (!bUseRangedAttack || !TargetCharacter) return;
	if (!RangedProjectileClass) return; 

	FVector MuzzleLocation;
	FRotator MuzzleRotation;

	USkeletalMeshComponent* MeshComp = GetMesh();

	if (MeshComp && SocketName != NAME_None && MeshComp->DoesSocketExist(SocketName))
	{
		MuzzleLocation = MeshComp->GetSocketLocation(SocketName);
		MuzzleRotation = (TargetCharacter->GetActorLocation() - MuzzleLocation).Rotation(); 
	}
	else
	{
		// 소켓이 없으면: 임시 위치(앞쪽+위쪽)에서 발사
		MuzzleLocation = GetActorLocation() + GetActorForwardVector() * 100.0f + FVector(0, 0, 50.0f);
		MuzzleRotation = (TargetCharacter->GetActorLocation() - MuzzleLocation).Rotation();
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;                
	SpawnParams.Instigator = GetInstigator(); 

	ABossProjectile* Projectile = GetWorld()->SpawnActor<ABossProjectile>(
		RangedProjectileClass,
		MuzzleLocation,
		MuzzleRotation,
		SpawnParams
	);

	if (!Projectile) return;

	AController* BossController = GetController();

	Projectile->InitProjectile(RangedDamage, BossController);
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
}

// 원거리 공격 끝나면 이동 풀기
void ABossBase::OnRangedMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == RangedAttackMontage)
	{
		UnlockMovement();
	}
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

			// 몽타주 끝나면 이동 잠금 해제
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

	bIsExecutingPattern = true;
	CurrentPattern = Pattern;

	const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
	LastPatternUseTime.FindOrAdd(Pattern) = Now;

	//이동 중지
	LockMovement();

	// 몽타주 있으면 재생, 실제 패턴은 노티파이에서 처리
	if (Data->Montage)
	{
		if (UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr)
		{
			if (!AnimInstance->Montage_IsPlaying(Data->Montage))
			{
				AnimInstance->Montage_Play(Data->Montage);
			}
			return;
		}
	}

	
	switch (Pattern)
	{
	case EBossPattern::Pattern1:
	case EBossPattern::Pattern2:
	case EBossPattern::Pattern3:
	case EBossPattern::Pattern4:
	case EBossPattern::Pattern5:
		PerformMeleeAttack();	//	실제 데미지는 ApplyMeleeDamage 노티파이에서
		break;

	case EBossPattern::Pattern6:
	case EBossPattern::Pattern7:
		ApplyRangedAttackFromSocket(RightHandSocketName);
		break;

	case EBossPattern::Pattern8:
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

	AnimInstance->Montage_Play(HitReactMontage);
	LastHitReactTime = Now;
}



void ABossBase::FinishCurrentPattern()
{
	UnlockMovement();

	bIsExecutingPattern = false;
	CurrentPattern = EBossPattern::None;

	if (CurrentState == EBossState::Attack)
	{
		SetBossState(EBossState::Chase);
	}
}



