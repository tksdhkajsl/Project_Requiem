// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Player/Components/LockOnComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Characters/BaseCharacter.h"
#include "Characters/Enemy/EnemyCharacter.h"
#include "Characters/LastBossCharacter/LastBossCharacter.h"
#include "BossBase/BossBase.h"
#include "Stats/StatComponent.h"

ULockOnComponent::ULockOnComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// [추가] 12/29, 감지용 스피어 생성
	DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphere"));
	DetectionSphere->SetSphereRadius(SearchRadius);
	DetectionSphere->SetCollisionProfileName(TEXT("Trigger"));
}

void ULockOnComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (OwnerCharacter)
	{
		PlayerController = Cast<APlayerController>(OwnerCharacter->GetController());
		// [추가] 12/29, 캐릭터 몸체에 스피어 부착 (캐릭터 따라다니게)
		if (USceneComponent* Root = OwnerCharacter->GetRootComponent())
		{
			DetectionSphere->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);
		}
		// 반지름 적용
		DetectionSphere->SetSphereRadius(SearchRadius);
	}
	// [추가] 12/29, 이벤트 바인딩
	// 적이 들어오거나 나갈 때만 함수가 실행됨
	if (DetectionSphere)
	{
		DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &ULockOnComponent::OnDetectionSphereBeginOverlap);
		DetectionSphere->OnComponentEndOverlap.AddDynamic(this, &ULockOnComponent::OnDetectionSphereEndOverlap);
	}
}

void ULockOnComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsLockedOn)
	{
		UpdateTargetLock(DeltaTime);
	}
}

// [추가] 12/29, 적이 범위에 들어오면 목록에 추가
void ULockOnComponent::OnDetectionSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == OwnerCharacter) return;

	// 이미 목록에 있으면 무시
	if (CandidateTargets.Contains(OtherActor)) return;

	// 적군인지 확인하고 목록에 추가
	if (IsValidTarget(OtherActor))
	{
		CandidateTargets.Add(OtherActor);
	}
}

// [추가] 12/29, 적이 범위 밖으로 나가면 목록에서 제거
void ULockOnComponent::OnDetectionSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && CandidateTargets.Contains(OtherActor))
	{
		CandidateTargets.Remove(OtherActor);
	}
}

// [추가] 12/29, 타겟 검사
bool ULockOnComponent::IsValidTarget(AActor* TargetActor) const
{
	if (!TargetActor) return false;

	// 1. 보스 체크
	if (ABossBase* Boss = Cast<ABossBase>(TargetActor))
	{
		return (Boss->CurrentState != EBossState::Dead && Boss->CurrentHP > 0.0f);
	}
	// 2. 일반 적 & 라스트 보스 체크
	else if (ABaseCharacter* BaseChar = Cast<ABaseCharacter>(TargetActor))
	{
		bool bIsEnemy = BaseChar->IsA(AEnemyCharacter::StaticClass());
		bool bIsLastBoss = BaseChar->IsA(ALastBossCharacter::StaticClass());

		if (bIsEnemy || bIsLastBoss)
		{
			if (BaseChar->GetStatComponent() && BaseChar->GetStatComponent()->GetStatCurrent(EFullStats::Health) > 0.f)
			{
				if (AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(BaseChar))
				{
					return (Enemy->CurrentState != EEnemyState::Dead);
				}
				return true;
			}
		}
	}
	return false;
}

void ULockOnComponent::ToggleLockOn()
{
	if (bIsLockedOn)
	{
		EndLockOn();
	}
	else
	{
		StartLockOn();
	}
}

void ULockOnComponent::StartLockOn()
{
	AActor* NewTarget = FindBestTarget();

	if (NewTarget)
	{
		CurrentTarget = NewTarget;
		bIsLockedOn = true;

		// 락온 시: 캐릭터가 항상 적을 바라보며 이동
		if (OwnerCharacter)
		{
			OwnerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
			OwnerCharacter->bUseControllerRotationYaw = true;
		}
	}
}

void ULockOnComponent::EndLockOn()
{
	bIsLockedOn = false;
	CurrentTarget = nullptr;

	// [이동 방식 원상복구]
	// 락온 해제 시: 이동하는 방향으로 캐릭터가 회전
	if (OwnerCharacter)
	{
		OwnerCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
		OwnerCharacter->bUseControllerRotationYaw = false;
	}
}

AActor* ULockOnComponent::FindBestTarget()
{
	// [수정 전]
	//if (!OwnerCharacter || !PlayerController) return nullptr;

	//FVector Start = OwnerCharacter->GetActorLocation();
	//TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	//ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn)); // Pawn만 검색

	//TArray<AActor*> ActorsToIgnore;
	//ActorsToIgnore.Add(OwnerCharacter);

	//TArray<AActor*> OutActors;

	//// 내 주변 구체 범위 내의 모든 Pawn 찾기
	//UKismetSystemLibrary::SphereOverlapActors(
	//	GetWorld(),
	//	Start,
	//	SearchRadius,
	//	ObjectTypes,
	//	ABaseCharacter::StaticClass(), // BaseCharacter 상속받은 애들만
	//	ActorsToIgnore,
	//	OutActors
	//);

	//AActor* BestTarget = nullptr;
	//float ClosestDot = -1.0f; // -1(뒤) ~ 1(앞)

	//for (AActor* Actor : OutActors)
	//{
	//	ABaseCharacter* Enemy = Cast<ABaseCharacter>(Actor);
	//	// 죽은 적은 무시
	//	if (!Enemy || !Enemy->GetStatComponent() || Enemy->GetStatComponent()->GetStatCurrent(EFullStats::Health) <= 0.f)
	//		continue;

	//	// 2. 화면 중앙에 가장 가까운 적 찾기 (내적 이용)
	//	FVector ToTarget = (Actor->GetActorLocation() - Start).GetSafeNormal();
	//	FVector CameraForward = PlayerController->PlayerCameraManager->GetCameraRotation().Vector();

	//	float Dot = FVector::DotProduct(CameraForward, ToTarget);

	//	// 화면 앞쪽(0.5 이상)에 있고, 이전에 찾은 애보다 더 중앙에 가까우면 선택
	//	if (Dot > 0.5f && Dot > ClosestDot)
	//	{
	//		ClosestDot = Dot;
	//		BestTarget = Actor;
	//	}
	//}

	//return BestTarget;

	// [수정] 12/29, for (AActor* Actor : OutActors)를 안 쓰기 위해서 바꿈

	if (!OwnerCharacter || !PlayerController) return nullptr;

	// 후보가 없으면 빠른 리턴
	if (CandidateTargets.Num() == 0) return nullptr;

	AActor* BestTarget = nullptr;
	float ClosestDot = -1.0f;

	FVector Start = OwnerCharacter->GetActorLocation();
	FVector CameraForward = PlayerController->PlayerCameraManager->GetCameraRotation().Vector();

	// [중요] 배열을 역순으로 돌면서 검사 (삭제 시 안전성을 위해 역순 권장)
	for (int32 i = CandidateTargets.Num() - 1; i >= 0; --i)
	{
		AActor* Candidate = CandidateTargets[i];

		// 1. 범위 안에는 있는데 그 사이에 죽었을 수도 있음 -> 재검사
		if (!IsValidTarget(Candidate))
		{
			CandidateTargets.RemoveAt(i); // 죽었으면 목록에서 삭제하고 패스
			continue;
		}

		// 2. 가장 중앙에 있는 적 찾기 (내적)
		FVector ToTarget = (Candidate->GetActorLocation() - Start).GetSafeNormal();
		float Dot = FVector::DotProduct(CameraForward, ToTarget);

		if (Dot > 0.5f && Dot > ClosestDot)
		{
			ClosestDot = Dot;
			BestTarget = Candidate;
		}
	}

	return BestTarget;
}

void ULockOnComponent::UpdateTargetLock(float DeltaTime)
{
	// 타겟 유효성 검사 (사라졌으면 락온 해제)
	if (!IsValid(CurrentTarget))
	{
		EndLockOn();
		return;
	}
	
	/// 코드 변경 필요 : 12/23(보스 1,2는 StatComponent 사용하지 않음)
	// [수정 전]
	//ABaseCharacter* Enemy = Cast<ABaseCharacter>(CurrentTarget);
	//// 타겟 유효성 검사 (적이 죽었으면 락온 해제)
	//if (Enemy && Enemy->GetStatComponent()->GetStatCurrent(EFullStats::Health) <= 0.f)
	//{
	//	EndLockOn();
	//	return;
	//}

	// [수정] 12/29, 타겟 사망 여부 확인 (IsValidTarget 재활용)
	if (!IsValidTarget(CurrentTarget))
	{
		EndLockOn();
		return;
	}

	// 거리 체크 (너무 멀어지면 해제)
	float Distance = FVector::Dist(OwnerCharacter->GetActorLocation(), CurrentTarget->GetActorLocation());
	if (Distance > SearchRadius * 1.2f) // 약간의 여유(Hysteresis) 제공
	{
		EndLockOn();
		return;
	}

	// [카메라 회전 로직]
	if (PlayerController)
	{
		FVector TargetLoc = CurrentTarget->GetActorLocation();
		TargetLoc.Z -= 30.0f; // 시선을 살짝 낮춤

		FVector LookDir = TargetLoc - OwnerCharacter->GetActorLocation(); // 카메라 위치가 아니라 캐릭터 위치 기준이 더 안정적일 수 있음
		// 정확히는 "카메라가 적을 보게" 해야 하므로:
		// FVector CamLoc = PlayerController->PlayerCameraManager->GetCameraLocation();
		// FVector LookDir = TargetLoc - CamLoc;

		FRotator TargetRot = LookDir.Rotation();
		FRotator CurrentRot = PlayerController->GetControlRotation();

		// 부드럽게 회전
		FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetRot, DeltaTime, InterpolationSpeed);

		// 너무 위/아래로 꺾이지 않게 제한
		NewRot.Pitch = FMath::Clamp(NewRot.Pitch, -45.0f, 45.0f);

		PlayerController->SetControlRotation(NewRot);
	}
}
