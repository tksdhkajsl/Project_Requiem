// Fill out your copyright notice in the Description page of Project Settings.



#include "Characters/Enemy/EnemyCharacter.h"
#include "Stats/StatComponent.h"            //
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"

AEnemyCharacter::AEnemyCharacter()
{
    // ABaseCharacter 생성자에서 StatComponent가 이미 생성됨
    PrimaryActorTick.bCanEverTick = true;

    Tags.Add(TEXT("Enemy"));
    GetCharacterMovement()->bOrientRotationToMovement = true;
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->MaxWalkSpeed = 350.0f;

    MonsterName = FText::FromString(TEXT("Skeleton Minion")); // 기본 이름 설정
}

void AEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();

    // 1. 스탯 초기화 (BaseCharacter의 StatComponent 사용)
    if (StatComponent)
    {
        StatComponent->InitializeStatsComponent(); //
    }

    // 2. 초기 위치 저장 및 상태 설정
    HomeLocation = GetActorLocation();
    CurrentState = EEnemyState::Patrol;
}

// =================================================================
// 피격 로직 (BaseCharacter 연동)
// =================================================================
float AEnemyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    // 이미 죽었으면 처리 안 함
    if (CurrentState == EEnemyState::Dead) return 0.0f;

    // 1. 부모 클래스(ABaseCharacter) 로직 실행 (필요시)
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    // 2. 마지막 공격자 저장 (경험치 분배용) - BaseCharacter에 선언됨
    LastHitInstigator = EventInstigator;

    // 3. StatComponent를 통한 데미지 처리
    if (StatComponent)
    {
        // 방어력 적용
        float Defense = StatComponent->GetStatCurrent(EFullStats::PhysicalDefense);
        ActualDamage = FMath::Max(0.0f, ActualDamage - Defense);

        // 체력 감소
        StatComponent->ChangeStatCurrent(EFullStats::Health, -ActualDamage);

        // 4. 사망 체크
        if (StatComponent->GetStatCurrent(EFullStats::Health) <= KINDA_SMALL_NUMBER)
        {
            Die();
        }
        else
        {
            // 맞았으면 추격 상태로 전환 (어그로)
            if (CurrentState != EEnemyState::Attack)
            {
                CurrentState = EEnemyState::Chase;
            }
        }
    }
    return ActualDamage;
}

// =================================================================
// 사망 처리 (BaseCharacter.h 주석 참고)
// =================================================================
void AEnemyCharacter::Die()
{
    if (CurrentState == EEnemyState::Dead) return;

    // 1. 상태 변경
    CurrentState = EEnemyState::Dead;

    // 2. 경험치 보상 지급 (BaseCharacter.h의 주석 로직 구현)
    if (LastHitInstigator.IsValid())
    {
        APawn* KillerPawn = LastHitInstigator->GetPawn();
        // ABaseCharacter 혹은 APlayerCharacter로 캐스팅하여 경험치 함수 호출
        if (ABaseCharacter* PlayerBase = Cast<ABaseCharacter>(KillerPawn))
        {
            // BaseCharacter에 AddExp가 정의되어 있음
            PlayerBase->AddExp(ExpReward);
            UE_LOG(LogTemp, Log, TEXT("Granted %f Exp to Player"), ExpReward);
        }
    }

    // 3. 충돌 및 이동 비활성화
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    if (AAIController* AIC = Cast<AAIController>(GetController()))
    {
        AIC->StopMovement();
    }

    // 4. 리스폰 시스템을 위해 액터를 파괴하지 않고 숨김 처리
    SetActorHiddenInGame(true);

    // 5. 부모 Die 호출 (애니메이션 재생 등이 있다면 여기서 수행)
    Super::Die();
}

// =================================================================
// 리스폰 (Reset)
// =================================================================
void AEnemyCharacter::ResetEnemy()
{
    // 1. 위치 및 회전 초기화
    SetActorLocation(HomeLocation);
    SetActorRotation(FRotator::ZeroRotator);

    // 2. 스탯 완전 회복
    if (StatComponent)
    {
        // InitializeStatsComponent를 다시 부르거나, Max값으로 채움
        float MaxHP = StatComponent->GetStatMax(EFullStats::Health);
        StatComponent->ChangeStatCurrent(EFullStats::Health, MaxHP); // 풀피
    }

    // 3. 상태 및 플래그 초기화
    CurrentState = EEnemyState::Patrol;
    LastHitInstigator = nullptr;

    // 4. 숨김 해제 및 충돌 복구
    SetActorHiddenInGame(false);
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    // 5. AI 재가동 (필요하다면)
    MoveToRandomPatrolPoint();
}

// =================================================================
// AI 및 공격 
// =================================================================
void AEnemyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (CurrentState == EEnemyState::Dead) return;

    UpdateAIState();
}

void AEnemyCharacter::UpdateAIState()
{
    ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    AAIController* AIC = Cast<AAIController>(GetController());
    if (!Player || !AIC) return;

    // 플레이어가 죽었으면 배회로 복귀
    ABaseCharacter* BasePlayer = Cast<ABaseCharacter>(Player);
    // BaseCharacter에 IsDead() 같은 게 없다면 HP로 체크하거나 직접 구현 필요
    // 여기서는 거리만 체크

    float Distance = FVector::Dist(GetActorLocation(), Player->GetActorLocation());

    if (Distance <= AttackRange)
    {
        CurrentState = EEnemyState::Attack;
    }
    else if (Distance <= DetectionRange)
    {
        CurrentState = EEnemyState::Chase;
    }
    else
    {
        CurrentState = EEnemyState::Patrol;
    }

    switch (CurrentState)
    {
    case EEnemyState::Attack:
        AIC->StopMovement();
        {
            FVector Dir = (Player->GetActorLocation() - GetActorLocation()).GetSafeNormal();
            FRotator LookRot = FRotator(0, Dir.Rotation().Yaw, 0);
            SetActorRotation(FMath::RInterpTo(GetActorRotation(), LookRot, GetWorld()->GetDeltaSeconds(), 10.0f));

            bool bCanAttack = !GetWorldTimerManager().IsTimerActive(AttackTimerHandle)
                && !GetMesh()->GetAnimInstance()->IsAnyMontagePlaying();
            if (bCanAttack) PerformAttack();
        }
        break;

    case EEnemyState::Chase:
        AIC->MoveToActor(Player, AttackRange * 0.9f);
        break;

    case EEnemyState::Patrol:
        if (AIC->GetMoveStatus() == EPathFollowingStatus::Idle)
        {
            MoveToRandomPatrolPoint();
        }
        break;
    }
}

void AEnemyCharacter::MoveToRandomPatrolPoint()
{
    AAIController* AIC = Cast<AAIController>(GetController());
    UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());

    if (AIC && NavSystem)
    {
        FNavLocation RandomPt;
        if (NavSystem->GetRandomReachablePointInRadius(HomeLocation, PatrolRadius, RandomPt))
        {
            AIC->MoveToLocation(RandomPt.Location);
        }
    }
}

void AEnemyCharacter::PerformAttack()
{
    if (AttackMontage) PlayAnimMontage(AttackMontage);

    // 공격 타이밍에 맞춰 데미지를 주는 것이 좋으나, 여기서는 단순 호출
    ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (Player)
    {
        float Dist = FVector::Dist(GetActorLocation(), Player->GetActorLocation());
        if (Dist <= AttackRange * 1.5f)
        {
            // BaseCharacter의 Attack 함수 사용
            // Attack 내부에서 데미지 처리를 할 수도 있고, 
            // 여기서는 ApplyDamage를 직접 부르거나 Attack()을 활용

            // 만약 BaseCharacter::Attack이 구현되어 있다면:
            Attack(Player, GetAttackDamage());
        }
    }
    GetWorldTimerManager().SetTimer(AttackTimerHandle, AttackDelay, false);
}

float AEnemyCharacter::GetAttackDamage() const
{
    if (StatComponent)
        return StatComponent->GetStatCurrent(EFullStats::PhysicalAttack);
    return 10.0f;
}