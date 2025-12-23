#include "Characters/Enemy/EnemyCharacter.h"
#include "Stats/StatComponent.h"            //
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "Components/WidgetComponent.h"
#include "UI/EnemyCharacter/EnemyCharacterHPWidget.h"
#include "Characters/Player/Character/PlayerCharacter.h" // 플레이어 캐스팅용
#include "DrawDebugHelpers.h" // 디버그 라인용

AEnemyCharacter::AEnemyCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    Tags.Add(TEXT("Enemy"));
    GetCharacterMovement()->bOrientRotationToMovement = true;
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->MaxWalkSpeed = 350.0f;

    MonsterName = FText::FromString(TEXT("Skeleton Minion"));

    // HP 위젯 설정
    HPWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPWidgetComponent"));
    HPWidgetComponent->SetupAttachment(GetRootComponent());
    HPWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
    HPWidgetComponent->SetDrawSize(FVector2D(100.f, 15.f));
    HPWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
    HPWidgetComponent->SetTickWhenOffscreen(false);
}

void AEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (StatComponent) StatComponent->InitializeStatsComponent();

    HomeLocation = GetActorLocation();
    HomeRotation = GetActorRotation();
    CurrentState = EEnemyState::Patrol;

    if (HPWidgetComponent)
    {
        HPWidgetComponent->InitWidget();
        HPWidget = Cast<UEnemyCharacterHPWidget>(HPWidgetComponent->GetUserWidgetObject());
        if (HPWidget && StatComponent)
        {
            HPWidget->UpdateHP(StatComponent->GetStatCurrent(EFullStats::Health),
                StatComponent->GetStatMax(EFullStats::Health));
        }
    }
}

// =================================================================
// Tick (UI 빌보드 및 AI)
// =================================================================
void AEnemyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (CurrentState == EEnemyState::Dead) return;

    // HP바 빌보드 및 최적화
    if (HPWidgetComponent)
    {
        APlayerController* PC = GetWorld()->GetFirstPlayerController();
        if (PC)
        {
            FVector CamLoc; FRotator CamRot;
            PC->GetPlayerViewPoint(CamLoc, CamRot);

            float Dist = FVector::Dist(CamLoc, GetActorLocation());
            bool bShow = (Dist < 2000.f) && (StatComponent->GetStatCurrent(EFullStats::Health) > 0.f);

            if (HPWidgetComponent->IsVisible() != bShow)
                HPWidgetComponent->SetVisibility(bShow);

            if (bShow)
            {
                FRotator LookAt = (CamLoc - HPWidgetComponent->GetComponentLocation()).Rotation();
                LookAt.Pitch = 0.f; LookAt.Roll = 0.f;
                HPWidgetComponent->SetWorldRotation(LookAt);
            }
        }
    }

    UpdateAIState();
}

// =================================================================
// 피격 및 사망
// =================================================================
float AEnemyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    if (CurrentState == EEnemyState::Dead) return 0.0f;

    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    LastHitInstigator = EventInstigator;

    if (StatComponent)
    {
        float Defense = StatComponent->GetStatCurrent(EFullStats::PhysicalDefense);
        ActualDamage = FMath::Max(0.0f, ActualDamage - Defense);

        StatComponent->ChangeStatCurrent(EFullStats::Health, -ActualDamage);

        // UI 갱신
        if (HPWidget)
        {
            HPWidget->UpdateHP(StatComponent->GetStatCurrent(EFullStats::Health),
                StatComponent->GetStatMax(EFullStats::Health));
        }

        // 사망 처리
        if (StatComponent->GetStatCurrent(EFullStats::Health) <= KINDA_SMALL_NUMBER)
        {
            HPWidgetComponent->SetVisibility(false);
            Die();
        }
        else if (CurrentState != EEnemyState::Attack)
        {
            CurrentState = EEnemyState::Chase; // 피격 시 추격
        }
    }
    return ActualDamage;
}

void AEnemyCharacter::Die()
{
    if (CurrentState == EEnemyState::Dead) return;
    CurrentState = EEnemyState::Dead;

    if (LastHitInstigator.IsValid())
    {
        if (ABaseCharacter* PlayerBase = Cast<ABaseCharacter>(LastHitInstigator->GetPawn()))
        {
            PlayerBase->AddExp(ExpReward);
        }
    }

    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    if (AAIController* AIC = Cast<AAIController>(GetController())) AIC->StopMovement();

    SetActorHiddenInGame(true); // Destroy 대신 숨김
    Super::Die();
}

// =================================================================
// 리스폰 (Reset)
// =================================================================
void AEnemyCharacter::ResetEnemy()
{
    SetActorLocation(HomeLocation);
    SetActorRotation(HomeRotation);

    if (StatComponent)
    {
        float MaxHP = StatComponent->GetStatMax(EFullStats::Health);
        StatComponent->ChangeStatCurrent(EFullStats::Health, MaxHP);

        // [중요] UI 100%로 갱신
        if (HPWidget) HPWidget->UpdateHP(MaxHP, MaxHP);
    }

    CurrentState = EEnemyState::Patrol;
    LastHitInstigator = nullptr;

    SetActorHiddenInGame(false);
    if (HPWidgetComponent) HPWidgetComponent->SetVisibility(true);

    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    MoveToRandomPatrolPoint();
}

// =================================================================
// AI 로직
// =================================================================
void AEnemyCharacter::UpdateAIState()
{
    // 1. 필수 포인터 가져오기
    ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    AAIController* AIC = Cast<AAIController>(GetController());

    if (!Player || !AIC) return;

    // =========================================================
    // [핵심] 공격 애니메이션 재생 중일 때의 처리
    // =========================================================
    if (GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
    {
        // 1. 이동 멈춤 (발이 미끄러지는 현상 방지)
        AIC->StopMovement();

        // 2. 플레이어 쪽으로 회전 (공격 도중 플레이어가 옆으로 피해도 바라보게 함)
        FVector Dir = (Player->GetActorLocation() - GetActorLocation()).GetSafeNormal();
        Dir.Z = 0.0f; // 높낮이 무시 (평면 회전)

        // 부드럽게 회전 (InterpSpeed 10~20 추천)
        FRotator TargetRot = FRotator(0, Dir.Rotation().Yaw, 0);
        SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRot, GetWorld()->GetDeltaSeconds(), 15.0f));

        // 3. 여기서 함수 종료! (아래의 추격/배회 로직이 실행되지 않도록 함)
        return;
    }

    // =========================================================
    // [일반 상태] 애니메이션이 안 나올 때만 실행됨
    // =========================================================

    // 거리 계산
    float Distance = FVector::Dist(GetActorLocation(), Player->GetActorLocation());

    // 1. 상태 결정 (State Decision)
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

    // 2. 상태별 행동 (State Action)
    switch (CurrentState)
    {
    case EEnemyState::Attack:
        AIC->StopMovement(); // 공격 사거리 내에서는 일단 정지

        // 공격 준비 (회전)
        {
            FVector Dir = (Player->GetActorLocation() - GetActorLocation()).GetSafeNormal();
            Dir.Z = 0.0f;
            FRotator TargetRot = FRotator(0, Dir.Rotation().Yaw, 0);
            SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRot, GetWorld()->GetDeltaSeconds(), 15.0f));
        }

        // 공격 실행 (쿨타임 X, 몽타주 X 일때만)
        // 위에서 IsAnyMontagePlaying 체크를 했으므로, 여기서는 쿨타임만 보면 됨
        if (!GetWorldTimerManager().IsTimerActive(AttackTimerHandle))
        {
            PerformAttack();
        }
        break;

    case EEnemyState::Chase:
        // 바짝 추격 (5.0f 앞까지)
        AIC->MoveToActor(Player, 5.0f);
        break;

    case EEnemyState::Patrol:
        // 목적지 도착했으면(Idle) 새로운 랜덤 위치로 이동
        // #include "Navigation/PathFollowingComponent.h" 필요
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

// =================================================================
// 공격 로직 (PerformAttack -> Animation -> Notify -> AttackHitCheck)
// =================================================================
void AEnemyCharacter::PerformAttack()
{
    // [중요] 여기서는 애니메이션만 실행! 데미지는 주지 않음.
    if (AttackMontage)
    {
        PlayAnimMontage(AttackMontage);
    }

    GetWorldTimerManager().SetTimer(AttackTimerHandle, AttackDelay, false);
}

void AEnemyCharacter::AttackHitCheck()
{
    // 1. 공격 범위 설정 (전방 100cm, 반경 50cm)
    FVector Start = GetActorLocation() + GetActorForwardVector() * 50.0f;
    FVector End = Start + GetActorForwardVector() * 100.0f;
    float Radius = 50.0f;

    // 2. 충돌 검사
    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    bool bHit = GetWorld()->SweepSingleByChannel(
        HitResult, Start, End, FQuat::Identity,
        ECC_Pawn, FCollisionShape::MakeSphere(Radius), Params
    );

    // [디버그] 공격 범위 표시 (빨간 원)
    // DrawDebugSphere(GetWorld(), End, Radius, 12, FColor::Red, false, 1.0f);

    // 3. 데미지 적용
    if (bHit && HitResult.GetActor())
    {
        // Tag 대신 Cast를 사용하여 플레이어인지 확인 (Tag 설정 안 해도 됨)
        APlayerCharacter* Player = Cast<APlayerCharacter>(HitResult.GetActor());

        if (Player)
        {
            UGameplayStatics::ApplyDamage(
                Player,
                GetAttackDamage(),
                GetController(),
                this,
                UDamageType::StaticClass()
            );
        }
    }
}

float AEnemyCharacter::GetAttackDamage() const
{
    if (StatComponent)
        return StatComponent->GetStatCurrent(EFullStats::PhysicalAttack);
    return 10.0f;
}