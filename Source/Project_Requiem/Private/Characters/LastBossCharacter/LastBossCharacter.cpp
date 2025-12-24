#include "Characters/LastBossCharacter/LastBossCharacter.h"
#include "ComponentSystems/Public/Stats/StatComponent.h"
#include "Components/SceneComponent.h"

#pragma region 보스방 도어
#include "Mics/Boss/BossDoor.h"
#include "Kismet/GameplayStatics.h"
#pragma endregion

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

	InitialLocation = GetActorLocation();
}

void ALastBossCharacter::Move(const FVector& Direction, float Value)
{
	Super::Move(Direction, Value);

	Move(Direction, Value);
}

void ALastBossCharacter::ReceiveDamage(float DamageAmount)
{
	GetStatComponent()->CurrHP -= DamageAmount;

	OnApplyDamage.Broadcast(GetStatComponent()->CurrHP, GetStatComponent()->MaxHP);
	OnBossStatUpdated.Broadcast(GetStatComponent()->CurrHP, GetStatComponent()->MaxHP, FText::FromString(TEXT("최종 보스")));

	Super::ReceiveDamage(DamageAmount);
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

#pragma region 보스방 도어
void ALastBossCharacter::ActivateBossBattle()
{
	// 이하 코드는 수도(의사)코드니 본인들의 로직에 맞게 변경 요망.
	// AI 활성화 (팀원이 쓰는 AIController 변수명 확인)
	//if (AAIController* AIC = Cast<AAIController>(GetController())) {
	//	AIC->GetBrainComponent()->RestartLogic(); // 비헤이비어 트리 다시 시작
	//}
	// 공격성, 이동 속도 등 전투 상태로 변경
	//GetCharacterMovement()->MaxWalkSpeed = 600.f;
	// UI 띄우기 등 추가 (예: Widget->SetVisibility(Visible))

	// 여기서 최초로 broadcast로 2가지
	// 1. 현재 체력, Max체력 => 체력 깍일때 마다, 무조건 broadcast
	// 2. 보스몹 이름(FText) => 굳이 계속 하면 메모리 아까우니 한번만

	OnBossStatUpdated.Broadcast(GetStatComponent()->CurrHP, GetStatComponent()->MaxHP, FText::FromString(TEXT("최종 보스")));
	UE_LOG(LogTemp, Log, TEXT("ALastBossCharacter::ActivateBossBattle()"));
}

// 3. 플레이어 패배 시 호출 (다시 재우기)
void ALastBossCharacter::ResetBossToDefault()
{
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

	UE_LOG(LogTemp, Log, TEXT("ALastBossCharacter::ResetBossToDefault()"));
}
#pragma endregion