// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Player/Components/LockOnComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Characters/BaseCharacter.h"
#include "Stats/StatComponent.h"

ULockOnComponent::ULockOnComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void ULockOnComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (OwnerCharacter)
	{
		PlayerController = Cast<APlayerController>(OwnerCharacter->GetController());
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

		// 락온 시: 캐릭터가 항상 적을 바라보며 이동 (게걸음)
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
	if (!OwnerCharacter || !PlayerController) return nullptr;

	FVector Start = OwnerCharacter->GetActorLocation();
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn)); // Pawn만 검색

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(OwnerCharacter);

	TArray<AActor*> OutActors;

	// 1. 내 주변 구체 범위 내의 모든 Pawn 찾기
	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		Start,
		SearchRadius,
		ObjectTypes,
		ABaseCharacter::StaticClass(), // BaseCharacter 상속받은 애들만
		ActorsToIgnore,
		OutActors
	);

	AActor* BestTarget = nullptr;
	float ClosestDot = -1.0f; // -1(뒤) ~ 1(앞)

	for (AActor* Actor : OutActors)
	{
		ABaseCharacter* Enemy = Cast<ABaseCharacter>(Actor);
		// 죽은 적은 무시
		if (!Enemy || !Enemy->GetStatComponent() || Enemy->GetStatComponent()->GetStatCurrent(EFullStats::Health) <= 0.f)
			continue;

		// 2. 화면 중앙에 가장 가까운 적 찾기 (내적 이용)
		FVector ToTarget = (Actor->GetActorLocation() - Start).GetSafeNormal();
		FVector CameraForward = PlayerController->PlayerCameraManager->GetCameraRotation().Vector();

		float Dot = FVector::DotProduct(CameraForward, ToTarget);

		// 화면 앞쪽(0.5 이상)에 있고, 이전에 찾은 애보다 더 중앙에 가까우면 선택
		if (Dot > 0.5f && Dot > ClosestDot)
		{
			ClosestDot = Dot;
			BestTarget = Actor;
		}
	}

	return BestTarget;
}

void ULockOnComponent::UpdateTargetLock(float DeltaTime)
{
	// 타겟 유효성 검사 (사라졌거나 죽었거나)
	if (!IsValid(CurrentTarget))
	{
		EndLockOn();
		return;
	}

	ABaseCharacter* Enemy = Cast<ABaseCharacter>(CurrentTarget);
	if (Enemy && Enemy->GetStatComponent()->GetStatCurrent(EFullStats::Health) <= 0.f)
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
		TargetLoc.Z -= 50.0f; // 시선을 살짝 낮춤 (적의 배꼽 쯤)

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
