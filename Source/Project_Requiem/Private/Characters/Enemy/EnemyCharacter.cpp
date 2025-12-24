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

    // HP바 빌보드 처리
    if (HPWidgetComponent)
    {
        APlayerController* PC = GetWorld()->GetFirstPlayerController();
        if (PC)
        {
            FVector CamLoc; FRotator CamRot;
            PC->GetPlayerViewPoint(CamLoc, CamRot);

            float Dist = FVector::Dist(CamLoc, GetActorLocation());
            // 20m 이내이고 체력이 있을 때만 보임
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
            Die();
        }
        else if (CurrentState != EEnemyState::Attack)
        {
            // 맞으면 추격 시작 (패트롤 대기 중이었다면 취소됨)
            CurrentState = EEnemyState::Chase;
        }
    }
    return ActualDamage;
}

void AEnemyCharacter::Die()
{
    if (CurrentState == EEnemyState::Dead) return;
    CurrentState = EEnemyState::Dead;

    // 경험치 지급
    if (LastHitInstigator.IsValid())
    {
        if (ABaseCharacter* PlayerBase = Cast<ABaseCharacter>(LastHitInstigator->GetPawn()))
        {
            PlayerBase->AddExp(ExpReward);
        }
    }

    // AI 정지
    if (AAIController* AIC = Cast<AAIController>(GetController()))
    {
        AIC->StopMovement();
    }

    // 1. 캡슐 충돌 끄기
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // 2. 래그돌 켜기
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
    GetMesh()->SetSimulatePhysics(true);

    // 3. HP바 숨김
    if (HPWidgetComponent) HPWidgetComponent->SetVisibility(false);

    // 4. 3~5초 뒤 시체 숨기기 타이머 설정    
    GetWorldTimerManager().SetTimer(DeadTimerHandle, this, &AEnemyCharacter::HideBody, RagdollWaitTime, false);
}

//시체 안 보임 처리
void AEnemyCharacter::HideBody()
{
    SetActorHiddenInGame(true);
    GetMesh()->SetSimulatePhysics(false); // 물리 꺼서 최적화
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// =================================================================
// 리스폰 (Reset)
// =================================================================
void AEnemyCharacter::ResetEnemy()
{
    // 숨김 타이머 및 패트롤 타이머 취소
    GetWorldTimerManager().ClearTimer(DeadTimerHandle);
    GetWorldTimerManager().ClearTimer(PatrolTimerHandle);

    // 1. 래그돌 끄기 & 메시 복구
    GetMesh()->SetSimulatePhysics(false);
    GetMesh()->SetCollisionProfileName(TEXT("CharacterMesh")); // 원래 프로필
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

    // 메시를 캡슐 안으로 다시 정렬 (중요)
    GetMesh()->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
    GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -88.0f)); // 캐릭터에 맞는 Z값
    GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

    // 2. 캡슐 충돌 켜기
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    // 3. 위치 복구
    SetActorLocation(HomeLocation);
    SetActorRotation(HomeRotation);

    // 4. 스탯 및 UI 복구
    if (StatComponent)
    {
        float MaxHP = StatComponent->GetStatMax(EFullStats::Health);
        StatComponent->ChangeStatCurrent(EFullStats::Health, MaxHP);
        if (HPWidget) HPWidget->UpdateHP(MaxHP, MaxHP);
    }

    CurrentState = EEnemyState::Patrol;
    LastHitInstigator = nullptr;

    // 5. 다시 보이기
    SetActorHiddenInGame(false);
    if (HPWidgetComponent) HPWidgetComponent->SetVisibility(true);

    MoveToRandomPatrolPoint();
}

// =================================================================
// AI 로직
// =================================================================
void AEnemyCharacter::UpdateAIState()
{
    ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    AAIController* AIC = Cast<AAIController>(GetController());

    if (!Player || !AIC) return;

    // 공격 중이면 멈춤
    if (GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
    {
        AIC->StopMovement();
        // ... (회전 로직) ...
        return;
    }

    float Distance = FVector::Dist(GetActorLocation(), Player->GetActorLocation());

    // 1. 새로운 상태 결정 (NewState)
    EEnemyState NewState = CurrentState;

    if (Distance <= AttackRange)
    {
        NewState = EEnemyState::Attack;
    }
    else if (Distance <= DetectionRange)
    {
        NewState = EEnemyState::Chase;
    }
    else
    {
        NewState = EEnemyState::Patrol;
    }

    // =========================================================
    // [핵심] 상태 변경 감지 및 하울링 재생
    // =========================================================

    // "이전에는 추격 상태가 아니었는데(Patrol 등), 방금 추격 상태(Chase)가 되었다면?"
    if (CurrentState != EEnemyState::Chase && NewState == EEnemyState::Chase)
    {
        // 공격하다가 추격으로 바뀐 게 아니라, '순찰' 중에 발견했을 때만 울게 하려면:
        if (CurrentState == EEnemyState::Patrol)
        {
            if (DetectSound)
            {
                UGameplayStatics::PlaySoundAtLocation(this, DetectSound, GetActorLocation());
                // (선택 사항) 로그 출력
                // UE_LOG(LogTemp, Warning, TEXT("Howling!")); 
            }
        }
    }

    // 패트롤 타이머 취소 로직
    if (CurrentState == EEnemyState::Patrol && NewState != EEnemyState::Patrol)
    {
        GetWorldTimerManager().ClearTimer(PatrolTimerHandle);
    }

    // 2. 상태 적용
    CurrentState = NewState;

    // 상태별 행동
    switch (CurrentState)
    {
    case EEnemyState::Attack:
        AIC->StopMovement();
        // 공격 가능하면 수행
        if (!GetWorldTimerManager().IsTimerActive(AttackTimerHandle))
        {
            PerformAttack();
        }
        break;

    case EEnemyState::Chase:
        AIC->MoveToActor(Player, 5.0f); // 바짝 추격
        break;

    case EEnemyState::Patrol:
        if (AIC->GetMoveStatus() == EPathFollowingStatus::Idle)
        {
            // 도착했는데 타이머가 안 돌고 있다면 3~5초 대기 예약
            if (!GetWorldTimerManager().IsTimerActive(PatrolTimerHandle))
            {
                float WaitTime = FMath::RandRange(3.0f, 5.0f);
                GetWorldTimerManager().SetTimer(PatrolTimerHandle, this, &AEnemyCharacter::MoveToRandomPatrolPoint, WaitTime, false);
            }
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
    // 애니메이션만 재생 (데미지는 주지 않음)
    if (AttackMontage)
    {
        PlayAnimMontage(AttackMontage);
    }
    GetWorldTimerManager().SetTimer(AttackTimerHandle, AttackDelay, false);
}

void AEnemyCharacter::AttackHitCheck()
{
    // 1. 공격 범위 설정 
    FVector Start = GetActorLocation() + GetActorForwardVector() * 50.0f;
    FVector End = Start + GetActorForwardVector() * 100.0f;
    float Radius = 50.0f;

    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    // 2. 충돌 검사 (Sweep)
    bool bHit = GetWorld()->SweepSingleByChannel(
        HitResult, Start, End, FQuat::Identity,
        ECC_Pawn, FCollisionShape::MakeSphere(Radius), Params
    );

    // 3. 데미지 및 소리 적용
    if (bHit && HitResult.GetActor())
    {
        // 타격음 재생 로직
        // 허공에 휘두를 땐 소리가 안 나고, 무언가에 맞았을 때만 남
        if (MeleeHitSound)
        {
            // HitResult.Location : 정확히 맞은 지점에서 소리가 남
            UGameplayStatics::PlaySoundAtLocation(this, MeleeHitSound, HitResult.Location);
        }

        // 데미지 적용 
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