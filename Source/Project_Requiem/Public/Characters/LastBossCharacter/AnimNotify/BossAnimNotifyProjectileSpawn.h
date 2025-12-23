// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Characters/LastBossCharacter/Projectile/ProjectileBase.h"
#include "BossAnimNotifyProjectileSpawn.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_REQUIEM_API UBossAnimNotifyProjectileSpawn : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;


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
