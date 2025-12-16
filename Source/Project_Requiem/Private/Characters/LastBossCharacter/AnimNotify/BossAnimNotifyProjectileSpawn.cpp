// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/LastBossCharacter/AnimNotify/BossAnimNotifyProjectileSpawn.h"
#include "Characters/LastBossCharacter/LastBossCharacter.h"
#include "Stats/StatComponent.h"

void UBossAnimNotifyProjectileSpawn::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (!LastBoss.IsValid())
	{
		LastBoss = Cast<ALastBossCharacter>(MeshComp->GetOwner());
	}
	if (LastBoss.IsValid())
	{
		if (Projectile)
		{
			DoSpawnProjectile(Projectile);
		}
	}
}

void UBossAnimNotifyProjectileSpawn::DoSpawnProjectile(TSubclassOf<AProjectileBase> InSpawnActor)
{
	// 유효성 검사
	if (!LastBoss.IsValid() || !InSpawnActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("LastBoss가 없습니다"));
		UE_LOG(LogTemp, Warning, TEXT("InSpawnActor가 없습니다"));
		return;
	}
	UWorld* World = LastBoss->GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("World가 없습니다"));
		return;
	}

	// 기존에 스폰된 액터가 있으면 제거(중복 스폰 방지)
	if (SpawnedProjectile.IsValid())
	{
		SpawnedProjectile->Destroy();
		SpawnedProjectile = nullptr;
	}

	// 컴포넌트 이름으로 씬 컴포넌트를 찾음
	USceneComponent* SpawnComp = nullptr;
	if (SpawnSceneComponentName != NAME_None)
	{
		TArray<USceneComponent*> Components;
		LastBoss->GetComponents<USceneComponent>(Components);
		for (USceneComponent* Comp : Components)
		{
			if (Comp && Comp->GetFName() == SpawnSceneComponentName)
			{
				SpawnComp = Comp;
				break;
			}
		}
	}

	// 컴포넌트를 못찾았을 경우 폴백
	if (!SpawnComp)
	{
		// LastBossCharacter의 기본 씬 컴포넌트 연결
		SpawnComp = LastBoss->GetSceneComponent();
	}
	if (!SpawnComp)
	{
		// LastBossCharacter의 루트 컴포넌트 연결
		SpawnComp = LastBoss->GetRootComponent();
	}
	// 유효성 검사
	if (!IsValid(SpawnComp))
	{
		UE_LOG(LogTemp, Warning, TEXT("씬 컴포넌트를 찾지 못했습니다"));
		return;
	}

	if (SpawnComp)
	{
		// 스폰 준비 및 스폰
		FTransform SpawnTransform = SpawnComp->GetComponentTransform();
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = LastBoss.Get();
		SpawnParams.Instigator = Cast<APawn>(LastBoss.Get());
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		AActor* Spawned = World->SpawnActor<AActor>(
			InSpawnActor,
			SpawnTransform,
			SpawnParams
		);

		// 스폰된 액터 저장
		if (Spawned)
		{
			SpawnedProjectile = Spawned;

			if (SpawnedProjectile.IsValid())
			{
				//UE_LOG(LogTemp, Log, TEXT("스폰되었습니다"));
			}
		}
	}
}

