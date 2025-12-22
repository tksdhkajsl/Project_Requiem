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

	AddPatternMontage();

	LastBossSpawn(SpawnMontage);
}

void ALastBossCharacter::ReceiveDamage(float DamageAmount)
{
	if (bLastBossInvincible)
		return;

	GetStatComponent()->CurrHP -= DamageAmount;

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

	// 페이즈 증가
	Phase++;

	// 저장된 페이즈2 스켈레탈 메시로 변경
	GetMesh()->SetSkeletalMesh(PhaseTwoSkeletalMesh);

	// 페이즈 변경 몽타주 종료 후 피격 활성화
	bLastBossInvincible = false;

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
		UE_LOG(LogTemp, Warning, TEXT("PhaseChangeMontage가 없습니다"));
		return;
	}
}

void ALastBossCharacter::LastBossEndPhaseChage(UAnimMontage* Montage, bool bInterrupted)
{

	OnLastBossEndChangedPhase.Broadcast();
}

void ALastBossCharacter::LastBossDead(UAnimMontage* Montage)
{
	Phase = 0;

	GetMesh()->GetAnimInstance()->StopAllMontages(0.2f);

	bLastBossInvincible = true;

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

	// 보스 사망 델리게이트
	OnLastBossDead.Broadcast();

	Destroy();
}

void ALastBossCharacter::ActivateBossBattle()
{
	OnBossStatUpdated.Broadcast(GetStatComponent()->CurrHP, GetStatComponent()->MaxHP, BossName);

	LastBossSpawn(SpawnMontage);

	//// 이하 코드는 수도(의사)코드니 본인들의 로직에 맞게 변경 요망.
	//// AI 활성화 (팀원이 쓰는 AIController 변수명 확인)
	//if (AAIController* AIC = Cast<AAIController>(GetController())) {
	//	AIC->GetBrainComponent()->RestartLogic(); // 비헤이비어 트리 다시 시작
	//}
	//// 공격성, 이동 속도 등 전투 상태로 변경
	//GetCharacterMovement()->MaxWalkSpeed = 600.f;
	//// UI 띄우기 등 추가 (예: Widget->SetVisibility(Visible))
}

// 3. 플레이어 패배 시 호출 (다시 재우기)
void ALastBossCharacter::ResetBossToDefault()
{
	// 체력 초기화
	GetStatComponent()->CurrHP = GetStatComponent()->MaxHP;

	//// 이하 코드는 수도(의사)코드니 본인들의 로직에 맞게 변경 요망.
	//// AI 끄기
	//if (AAIController* AIC = Cast<AAIController>(GetController())) {
	//	AIC->GetBrainComponent()->StopLogic("Player Defeated");
	//}

	//// 위치 초기화 및 체력 회복
	//SetActorLocation(InitialLocation);
	//SetActorRotation(FRotator::ZeroRotator);

	//// 보스 스탯 컴포넌트가 있다면 (예시) => 체력 풀(Full)로 채우기
	//// StatComponent->SetHP(StatComponent->GetMaxHP());

	//// 애니메이션 몽타주 중지하고 Idle로
	//GetMesh()->GetAnimInstance()->StopAllMontages(0.2f);

	//// 플레이어 HUD초기화는 플레이어가 담당.
}
