// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Characters/LastBossCharacter/LastBossCharacter.h"
#include "ProjectileBase.generated.h"

UCLASS()
class PROJECT_REQUIEM_API AProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectileBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	void ActivateProjectile();

	void DeactivateProjectile();

	bool IsActiveProjectile();

protected:
	void ApplyDamageToActor();

	void MoveProjectile();

	void ResetProjectileState();


protected:
	// 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<class UCapsuleComponent> CapsuleComponent = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<class UStaticMeshComponent> MeshComponent = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Niagara")
	TObjectPtr<class UNiagaraComponent> Niagara = nullptr;

	// 투사체 정보
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setting")
	float Damagemagnification = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setting")
	float Speed = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setting")
	float lifeTime = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setting")
	bool SingleHitMode = false;

private:
	// 보스
	TWeakObjectPtr<ALastBossCharacter> LastBoss;

	// 투사체가 활성화 되었는지에대한 여부
	bool bActive = false;

	// 투사체의 지속시간 타이머핸들
	FTimerHandle LifeTimerHandle;
};
