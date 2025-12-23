// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/LastBossCharacter/LastBossCharacter.h"
#include "ComponentSystems/Public/Stats/StatComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "AIController.h"
#include "BrainComponent.h"

ALastBossCharacter::ALastBossCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	SpawnProjectileLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Projectile"));
	SpawnProjectileLocation->SetupAttachment(RootComponent);
}

void ALastBossCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 보스 초기화
	ResetLastBoss();

	// 테스트용 보스 스폰
	LastBossSpawn(SpawnMontage);
}

void ALastBossCharacter::ReceiveDamage(float DamageAmount)
{
	if (bLastBossInvincible)
		return;

	//GetStatComponent()->CurrHP -= DamageAmount;

	OnBossStatUpdated.Broadcast(GetStatComponent()->CurrHP, GetStatComponent()->MaxHP, BossName);

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
		// 페이즈 변경 함수 실행
		LastBossPhaseChage(PhaseChangeMontage);
	}
	else if (Phase == 2)
	{
		// 죽었을 때 함수 실행
		LastBossDead(DieMontage);		
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

void ALastBossCharacter::LastBossSpawn(UAnimMontage* Montage)
{
	if (Montage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		// 스폰시 몽타주 실행
		float Duration = GetMesh()->GetAnimInstance()->Montage_Play(Montage);
		// 몽타주 종료 이후 
		if (Duration > 0.0f)
		{
			FOnMontageEnded OnMontageEnded;
			OnMontageEnded.BindUObject(this, &ALastBossCharacter::LastBossEndSpawn);
			GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(OnMontageEnded, Montage);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnMontage가 없습니다"));
	}
}

void ALastBossCharacter::LastBossEndSpawn(UAnimMontage* Montage, bool bInterrupted)
{
	bLastBossInvincible = false;
	OnLastBossSpawn.Broadcast();
}

void ALastBossCharacter::LastBossPhaseChage(UAnimMontage* Montage)
{
	// 페이즈 넘어가는 동안 피격 비활성화
	bLastBossInvincible = true;

	// 체력 초기화
	GetStatComponent()->CurrHP = GetStatComponent()->MaxHP;

	// 페이즈 증가
	Phase++;

	// 페이즈 변경 델리게이트 
	OnLastBossChangedPhase.Broadcast();

	if (Montage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		// 페이즈 변경 시 몽타주 출력
		float Duration = GetMesh()->GetAnimInstance()->Montage_Play(Montage);
		// 페이즈 변경 몽타주 종료 이후
		if (Duration > 0.0f)
		{
			FOnMontageEnded OnMontageEnded;
			OnMontageEnded.BindUObject(this, &ALastBossCharacter::LastBossEndPhaseChage);
			GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(OnMontageEnded, Montage);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("LastBossPhaseChage : 몽타주가 없습니다"));
		return;
	}
}

void ALastBossCharacter::LastBossEndPhaseChage(UAnimMontage* Montage, bool bInterrupted)
{
	// 즉시 몽타주 정지(애니메이션 블루프린트가 구(이전) 메시에 접근하지 않도록)
	if (GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->StopAllMontages(0.2f);
	}

	// 교체 중 재피격 방지
	bLastBossInvincible = true;

	// 유효성 검사
	if (!GetMesh())
	{
		UE_LOG(LogTemp, Error, TEXT("LastBossEndPhaseChage: Mesh가 없음"));
		bLastBossInvincible = false;
		OnLastBossEndChangedPhase.Broadcast();
		return;
	}
	if (!PhaseTwoSkeletalMesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("LastBossEndPhaseChage: PhaseTwoSkeletalMesh가 Null이다"));
		bLastBossInvincible = false;
		OnLastBossEndChangedPhase.Broadcast();
		return;
	}
	// 스켈레톤 호환성 경고(필요시 에셋 교체 확인)
	if (GetMesh()->SkeletalMesh && GetMesh()->SkeletalMesh->GetSkeleton() != PhaseTwoSkeletalMesh->GetSkeleton())
	{
		UE_LOG(LogTemp, Warning, TEXT("LastBossEndPhaseChage: 현재 메시가 PhaseTwoSkeletalMesh가 아님"));
	}
	if (!IsValid(this) || !GetMesh() || !PhaseTwoSkeletalMesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("LastBossEndPhaseChage: swap aborted, invalid pointers"));
		bLastBossInvincible = false;
		OnLastBossEndChangedPhase.Broadcast();
		return;
	}

	// 메쉬 변경을 바로 하지 않고 아주 짧게 지연시켜 재진입 문제(델리게이트/애니메이션 BP)를 회피
	FTimerHandle SwapTimerHandle;
	GetWorldTimerManager().SetTimer(
		SwapTimerHandle,
		FTimerDelegate::CreateWeakLambda(this, [this]()
			{
				// 실제 메쉬 교체
				GetMesh()->SetSkeletalMesh(PhaseTwoSkeletalMesh);

				// 본/애니메이션 상태 강제 갱신
				GetMesh()->RefreshBoneTransforms();			// 애니메이션 블루프린트나 렌더가 이전 정보 접근 방지
				GetMesh()->SetForcedLOD(0);					// LOD강제해서 렌더/애니메이션이 올바른 버텍스/본 데이터로 동작하도록 보장
				GetMesh()->MarkRenderTransformDirty();		// 메시 교체로 인해 렌더러가 변환 최신값을 받아야 할 때 사용
				GetMesh()->MarkRenderStateDirty();			// 렌더 상태를 강재로 재생성(시각적 불일치, 크래시 방지)

				// 교체 완료 후 피격 허용
				bLastBossInvincible = false;

				OnLastBossEndChangedPhase.Broadcast();
			}),
		0.05f, // 0.05초 지연: 필요시 더 줄이거나 늘려 테스트
		false
	);
	
}

void ALastBossCharacter::LastBossDead(UAnimMontage* Montage)
{
	// 몽타주 출력 방지를 위한 페이즈 초기화
	Phase = 0;

	// 무적 활성화
	bLastBossInvincible = true;

	// 실행 중인 몽타주 정지
	GetMesh()->GetAnimInstance()->StopAllMontages(0.2f);

	// 보스 사망을 위한 AIController종료 델리게이트
	OnLastBossStop.Broadcast();

	if (Montage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		// 보스가 죽었을 때 몽타주 실행
		float Duration = GetMesh()->GetAnimInstance()->Montage_Play(Montage);
		// 페이즈 변경 몽타주 종료 이후
		if (Duration > 0.0f)
		{
			FOnMontageEnded OnMontageEnded;
			OnMontageEnded.BindUObject(this, &ALastBossCharacter::LastBossEndDead);
			GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(OnMontageEnded, Montage);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("DieMontage가 없습니다"));
	}
}

void ALastBossCharacter::LastBossEndDead(UAnimMontage* Montage, bool bInterrupted)
{
	// 경험치 드랍
	ApplyExp(DropExp);

	Destroy();
}

void ALastBossCharacter::ActivateBossBattle()
{
	OnBossStatUpdated.Broadcast(GetStatComponent()->CurrHP, GetStatComponent()->MaxHP, BossName);

	LastBossSpawn(SpawnMontage);
}


void ALastBossCharacter::ResetBossToDefault()
{
	// AIController 종료 델리게이트
	OnLastBossStop.Broadcast();

	// 위치 초기화
	SetActorLocation(SpawnLocation);
	SetActorRotation(FRotator::ZeroRotator);

	// 체력 회복
	GetStatComponent()->CurrHP = GetStatComponent()->MaxHP;

	// 몽타주 종료
	GetMesh()->GetAnimInstance()->StopAllMontages(0.2f);
}

void ALastBossCharacter::ResetLastBoss()
{
	// 몽타주 추가
	AddPatternMontage();

	// 스폰 위치 저장
	SpawnLocation = GetActorLocation();

	// 보스 스폰 시 무적 활성화
	bLastBossInvincible = true;
}

