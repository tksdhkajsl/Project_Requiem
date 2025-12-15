#include "BossBase/BossBase.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "BossBase/Projectile/BossProjectile.h"

// =========================
// Tick: 매 프레임
// =========================
void ABossBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 1) 쿨타임 누적(Attack 상태에서 “시간 체크”에 사용)
	TimeSinceLastMeleeAttack += DeltaTime;
	TimeSinceLastRangedAttack += DeltaTime;

	// 2) 현재 상태에 맞는 로직 실행(FSM)
	UpdateState(DeltaTime);
}

void ABossBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// 보스는 보통 입력을 안 쓰므로 비워둠
}

// =========================
// 생성자: 기본값 초기화/컴포넌트 세팅
// =========================
ABossBase::ABossBase()
{
	PrimaryActorTick.bCanEverTick = true;

	// CharacterMovement가 있다면 MaxWalkSpeed에 WalkSpeed 반영
	// (주의: 파생 클래스에서 WalkSpeed를 바꿔도 BeginPlay에서 다시 적용하니 최종은 BeginPlay 기준)
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = WalkSpeed;
	}
}

// =========================
// BeginPlay: 스폰 후 1회
// =========================
void ABossBase::BeginPlay()
{
	Super::BeginPlay();

	// 체력 초기화
	CurrentHP = MaxHP;

	// 플레이어 캐릭터 캐시(0번 플레이어)
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	TargetCharacter = Cast<ACharacter>(PlayerPawn);

	// 이동 속도 적용(파생 클래스에서 WalkSpeed 바꾼 값이 여기서 반영됨)
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = WalkSpeed;
	}

	// 시작 시 자동 추적 여부 + 타겟 존재 여부
	if (bAutoStartChase && TargetCharacter)
	{
		SetBossState(EBossState::Chase);
	}
	else
	{
		SetBossState(EBossState::Idle);
	}
}

// =========================
// SetBossState: 상태 변경 “단일 진입점”
// - 상태가 바뀔 때 필요한 1회성 처리(이동 정지, 몽타주 재생 등)를 여기서 한다.
// =========================
void ABossBase::SetBossState(EBossState NewState)
{
	// 같은 상태면 아무것도 안 함(중복 실행 방지)
	if (CurrentState == NewState)
	{
		return;
	}

	EBossState OldState = CurrentState;
	CurrentState = NewState;

	// 상태 진입 시 1회 처리들
	switch (CurrentState)
	{
	case EBossState::Dead:
	{
		// 이동 즉시 정지
		if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
		{
			MoveComp->StopMovementImmediately();
		}

		// 캡슐 콜리전 끔(바닥/플레이어와 부딪힘 방지)
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// 죽음 몽타주 재생
		if (DeathMontage)
		{
			if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
			{
				AnimInstance->Montage_Play(DeathMontage);
			}
		}

		// 5초 뒤 Destroy(시체 남기는 시간)
		FTimerHandle DestroyTimerHandle;
		GetWorldTimerManager().SetTimer(
			DestroyTimerHandle,
			this,
			&ABossBase::K2_DestroyActor,
			5.0f, false
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

		// 연출 중에는 이동 막기(강제 정지)
		if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
		{
			MoveComp->StopMovementImmediately();
			MoveComp->DisableMovement();
		}

		// 몽타주가 없으면(연출 없음) 즉시 페이즈 전환 종료
		if (!PhaseChangeMontage)
		{
			FinishPhaseChange();
		}
	}
	break;

	default:
		break;
	}

	// 상태 변경 이벤트 브로드캐스트(외부 UI/연출에서 사용)
	OnBossStateChanged.Broadcast(NewState, OldState);
}

// =========================
// UpdateState: FSM 업데이트(매 Tick)
// =========================
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
	// 예: 등장 모션, 포즈 유지, 대기 애니 등
}

void ABossBase::UpdateChase(float DeltaTime)
{
	// 타겟 없으면 Idle로
	if (!TargetCharacter)
	{
		SetBossState(EBossState::Idle);
		return;
	}

	const FVector BossLocation = GetActorLocation();
	const FVector TargetLocation = TargetCharacter->GetActorLocation();

	FVector ToTarget = TargetLocation - BossLocation;
	ToTarget.Z = 0.0f; // 지면 이동만(상하 무시)

	const float DistanceToTarget = ToTarget.Length();

	// Attack 진입 거리:
	// - 원거리 사용이면 RangedAttackRange
	// - 아니면 MeleeAttackRange
	const float AttackEnterRange = bUseRangedAttack ? RangedAttackRange : MeleeAttackRange;

	// 진입 거리 이하면 Attack 상태로 전환
	if (DistanceToTarget <= AttackEnterRange)
	{
		SetBossState(EBossState::Attack);
		return;
	}

	// 너무 가까우면 겹침 방지로 멈추기만(Chase 유지)
	if (DistanceToTarget <= StoppingDistance)
	{
		return;
	}

	ToTarget.Normalize();

	// 타겟 바라보게 회전(부드럽게 보간)
	const FRotator TargetRot = ToTarget.Rotation();
	const FRotator NewRot = FMath::RInterpTo(GetActorRotation(), TargetRot, DeltaTime, 5.0f);
	SetActorRotation(NewRot);

	// 전진(캐릭터 무브먼트 입력)
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

	// Attack 상태 유지 최대 거리:
	// - 원거리 사용이면 RangedAttackRange
	// - 아니면 MeleeAttackRange
	const float AttackMaxRange = bUseRangedAttack ? RangedAttackRange : MeleeAttackRange;

	// 너무 멀어지면 다시 Chase로
	if (DistanceToTarget > AttackMaxRange)
	{
		SetBossState(EBossState::Chase);
		return;
	}

	// 공격 중에도 타겟 바라보기(더 빠른 회전 보간)
	ToTarget.Normalize();
	const FRotator TargetRot = ToTarget.Rotation();
	const FRotator NewRot = FMath::RInterpTo(GetActorRotation(), TargetRot, DeltaTime, 10.0f);
	SetActorRotation(NewRot);

	// -------------------------
	// 공격 타입 결정 로직
	// - 근접 범위면 근접
	// - 근접 밖이면서 원거리 사용 + 원거리 범위면 원거리
	// -------------------------
	if (DistanceToTarget <= MeleeAttackRange)
	{
		// 근접 공격 쿨타임 체크
		if (TimeSinceLastMeleeAttack >= MeleeAttackInterval)
		{
			PerformMeleeAttack();         // 몽타주 재생(실제 데미지는 노티파이에서)
			TimeSinceLastMeleeAttack = 0.0f;
		}
	}
	else if (bUseRangedAttack && DistanceToTarget <= RangedAttackRange)
	{
		// 원거리 공격 쿨타임 체크
		if (TimeSinceLastRangedAttack >= RangedAttackInterval)
		{
			PerformRangedAttack();        // 몽타주 재생 or 즉시 발사
			TimeSinceLastRangedAttack = 0.0f;
		}
	}
}

void ABossBase::UpdatePhaseChange(float DeltaTime)
{
	// 보통 “연출 중”이라 로직 비우고
	// 애니 노티파이나 타이밍에서 FinishPhaseChange() 호출로 빠져나감
}

void ABossBase::UpdateDead(float DeltaTime)
{
	// 사망 상태 지속 로직(보통 비움)
}

// =========================
// TakeDamage: 데미지 처리 핵심
// - HP 감소
// - OnBossDamaged 브로드캐스트
// - 죽으면 Dead 처리 + OnBossDead
// - 살아있으면 페이즈 체크
// =========================
float ABossBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	// 기본 TakeDamage 동작(엔진 기본 처리) 호출 후 실제 데미지 반환
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// 이미 죽었거나 데미지 0이면 무시
	if (ActualDamage <= 0.0f || CurrentState == EBossState::Dead)
	{
		return 0.0f;
	}

	// HP 감소(0~MaxHP 사이로 clamp)
	CurrentHP = FMath::Clamp(CurrentHP - ActualDamage, 0.0f, MaxHP);

	// 피격 이벤트(HP UI 갱신 등에 사용)
	OnBossDamaged.Broadcast(CurrentHP, MaxHP);

	// -------------------------
	// 사망 처리
	// -------------------------
	if (CurrentHP <= 0.0f)
	{
		SetBossState(EBossState::Dead);

		// 여기 StopMovementImmediately는 SetBossState(Dead)에서도 이미 수행(중복이지만 안전)
		if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
		{
			MoveComp->StopMovementImmediately();
		}

		// 사망 보상 이벤트
		OnBossDead.Broadcast(EXP);

		return ActualDamage;
	}

	// -------------------------
	// 페이즈 체크(살아있을 때만)
	// -------------------------
	if (bUsePhaseSystem && CurrentPhase == 1 && MaxHP > 0.0f)
	{
		const float HPRatio = CurrentHP / MaxHP;

		// HP 비율이 임계값 이하이면 2페이즈로
		if (HPRatio <= Phase2StartHPRatio)
		{
			const int32 OldPhase = CurrentPhase;
			CurrentPhase = 2;

			// 외부에 페이즈 변경 알림
			OnBossPhaseChanged.Broadcast(CurrentPhase, OldPhase);

			// BP/파생클래스에서 훅(스탯 변경, 원거리 on 등)
			OnPhaseChanged(CurrentPhase, OldPhase);

			// 연출 상태로 전환(몽타주 재생 + 이동 잠금)
			SetBossState(EBossState::PhaseChange);
		}
	}

	return ActualDamage;
}

// =========================
// OnPhaseChanged_Implementation
// - 기본 구현: 2페이즈 진입 시 스탯 배수 적용
// =========================
void ABossBase::OnPhaseChanged_Implementation(int32 NewPhase, int32 OldPhase)
{
	if (NewPhase == 2 && bUsePhaseSystem)
	{
		WalkSpeed *= Phase2_WalkSpeedMultiplier;   // 이동속도 증가
		MeleeDamage *= Phase2_MeleeDamageMultiplier; // 데미지 증가

		// CharacterMovement 반영
		if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
		{
			MoveComp->MaxWalkSpeed = WalkSpeed;
		}
	}
}

// =========================
// FinishPhaseChange: 페이즈 연출 종료 후 상태 복귀
// =========================
void ABossBase::FinishPhaseChange()
{
	// “지금 PhaseChange 상태일 때만” 처리
	if (CurrentState == EBossState::PhaseChange)
	{
		if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
		{
			// DisableMovement() 했던 걸 다시 Walking으로
			MoveComp->SetMovementMode(MOVE_Walking);
		}

		// 다시 추적으로 복귀
		SetBossState(EBossState::Chase);
	}
}

// =========================
// ApplyMeleeDamage: 실제 데미지 적용(노티파이에서 호출)
// =========================
void ABossBase::ApplyMeleeDamage()
{
	if (CurrentState == EBossState::Dead) return; // 죽었으면 무시
	if (!TargetCharacter) return;                 // 타겟 없으면 무시

	const float Distance = FVector::Dist2D(GetActorLocation(), TargetCharacter->GetActorLocation());
	if (Distance > MeleeAttackRange) return;      // 범위 밖이면 “허공”으로 처리

	AController* BossController = GetController(); // 데미지 instigator로 넘길 컨트롤러

	UGameplayStatics::ApplyDamage(
		TargetCharacter,
		MeleeDamage,
		BossController, // EventInstigator
		this,           // DamageCauser(데미지 원인 액터)
		nullptr
	);
}

// 기본 원거리 발사(오른손 1발) 래퍼
void ABossBase::ApplyRangedAttack()
{
	ApplyRangedAttackFromSocket(RightHandSocketName);
}

// =========================
// PerformMeleeAttack: 근접 “시도”
// - 몽타주 재생만 담당
// - 실제 데미지는 ApplyMeleeDamage(AnimNotify)에서 들어감
// =========================
void ABossBase::PerformMeleeAttack()
{
	if (CurrentState == EBossState::Dead) return;
	if (!TargetCharacter) return;

	const float Distance = FVector::Dist2D(GetActorLocation(), TargetCharacter->GetActorLocation());
	if (Distance > MeleeAttackRange) return; // 범위 밖이면 재생 안 함

	// 공격 중에는 이동 멈춤(찌르기/베기 모션 안정)
	GetCharacterMovement()->StopMovementImmediately();

	// 근접 공격 몽타주 재생
	if (MeleeAttackMontage)
	{
		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			// 다른 몽타주 재생 중이면 중복 재생 방지
			if (!AnimInstance->IsAnyMontagePlaying())
			{
				AnimInstance->Montage_Play(MeleeAttackMontage);
			}
		}
	}
}

// =========================
// ApplyRangedAttackFromSocket: 소켓 기준 발사체 생성 + InitProjectile
// =========================
void ABossBase::ApplyRangedAttackFromSocket(FName SocketName)
{
	if (CurrentState == EBossState::Dead) return;
	if (!bUseRangedAttack || !TargetCharacter) return;
	if (!RangedProjectileClass) return; // 발사체 클래스 지정 안되면 발사 불가

	FVector MuzzleLocation;
	FRotator MuzzleRotation;

	USkeletalMeshComponent* MeshComp = GetMesh();

	// 소켓이 존재하면: 소켓 위치에서 발사
	if (MeshComp && SocketName != NAME_None && MeshComp->DoesSocketExist(SocketName))
	{
		MuzzleLocation = MeshComp->GetSocketLocation(SocketName);
		MuzzleRotation = (TargetCharacter->GetActorLocation() - MuzzleLocation).Rotation(); // 타겟 방향으로 회전
	}
	else
	{
		// 소켓이 없으면: 임시 위치(앞쪽+위쪽)에서 발사
		MuzzleLocation = GetActorLocation() + GetActorForwardVector() * 100.0f + FVector(0, 0, 50.0f);
		MuzzleRotation = (TargetCharacter->GetActorLocation() - MuzzleLocation).Rotation();
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;                // Owner 지정(발사체가 보스를 무시하게 만들 때도 사용)
	SpawnParams.Instigator = GetInstigator(); // 보스 instigator(없을 수도 있음)

	ABossProjectile* Projectile = GetWorld()->SpawnActor<ABossProjectile>(
		RangedProjectileClass,
		MuzzleLocation,
		MuzzleRotation,
		SpawnParams
	);

	if (!Projectile) return;

	// 데미지 instigator 컨트롤러(피해 처리에서 가해자 판정에 사용)
	AController* BossController = GetController();

	// 발사체에 데미지 + InstigatorController 주입
	Projectile->InitProjectile(RangedDamage, BossController);
}

// =========================
// PerformRangedAttack: 원거리 “시도”
// - 몽타주 있으면 몽타주 재생(노티파이가 ApplyRangedAttackFromSocket 호출)
// - 몽타주 없으면 즉시 발사(ApplyRangedAttack)
// =========================
void ABossBase::PerformRangedAttack()
{
	if (CurrentState == EBossState::Dead) return;
	if (!bUseRangedAttack || !TargetCharacter) return;

	// 몽타주가 없으면 즉발(디버그/간단형)
	if (!RangedAttackMontage)
	{
		ApplyRangedAttack();
		return;
	}

	// 발사 모션 중 이동 멈춤
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->StopMovementImmediately();
	}

	// 몽타주 재생(노티파이에서 실제 발사)
	if (UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr)
	{
		if (!AnimInstance->IsAnyMontagePlaying())
		{
			AnimInstance->Montage_Play(RangedAttackMontage);
		}
	}
}
