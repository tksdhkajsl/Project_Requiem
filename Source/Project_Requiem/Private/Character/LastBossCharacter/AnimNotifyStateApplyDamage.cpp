// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/LastBossCharacter/AnimNotifyStateApplyDamage.h"
#include "Character/LastBossCharacter/LastBossCharacter.h"
#include "Stats/StatComponent.h"

void UAnimNotifyStateApplyDamage::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

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

void UAnimNotifyStateApplyDamage::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	// 스폰된 Projectile 제거
	if (SpawnedProjectile.IsValid())
	{
		SpawnedProjectile->Destroy();
		SpawnedProjectile = nullptr;
		UE_LOG(LogTemp, Log, TEXT("스폰된 액터가 삭제되었습니다"));
	}
	Super::NotifyEnd(MeshComp, Animation, EventReference);
}

void UAnimNotifyStateApplyDamage::DoSpawnProjectile(TSubclassOf<AActor> InSpawnActor)
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

	// 컴포넌트 이름으로 씬 컴포넌트를 찾음
	USceneComponent* SpawnComp = nullptr;
	if (SpawnComponentName != NAME_None)
	{
		TArray<USceneComponent*> Components;
		LastBoss->GetComponents<USceneComponent>(Components);
		for (USceneComponent* Comp : Components)
		{
			if (Comp && Comp->GetFName() == SpawnComponentName)
			{
				SpawnComp = Comp;
				break;
			}
		}
	}

	// 컴포넌트를 못찾았을 경우 폴백
	if (!SpawnComp)
	{
		SpawnComp = LastBoss->GetSceneComponent();
	}
	if (!SpawnComp)
	{
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
			UE_LOG(LogTemp, Log, TEXT("스폰되었습니다"))
			SpawnedProjectile = Spawned;
	}
}
