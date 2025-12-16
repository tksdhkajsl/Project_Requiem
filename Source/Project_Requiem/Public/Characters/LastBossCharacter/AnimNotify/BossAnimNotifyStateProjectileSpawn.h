// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Characters/LastBossCharacter/Projectile/ProjectileBase.h"
#include "BossAnimNotifyStateProjectileSpawn.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_REQUIEM_API UBossAnimNotifyStateProjectileSpawn : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	virtual void NotifyBegin(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		float TotalDuration,
		const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;

	// 투사체 스폰 함수
	void DoSpawnProjectile(TSubclassOf<AProjectileBase> InSpawnActor);

protected:
	// 스폰할 액터
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting|SpawnActor")
	TSubclassOf<AProjectileBase> Projectile = nullptr;

	UPROPERTY(EditInstanceOnly, Category = "Setting|SpawnComponentName")
	FName SpawnSceneComponentName = NAME_None;

private:
	TWeakObjectPtr<class ALastBossCharacter> LastBoss = nullptr;

	// 스폰된 액터 저장용
	TWeakObjectPtr<AActor> SpawnedProjectile = nullptr;
};
