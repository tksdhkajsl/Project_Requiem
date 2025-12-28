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
	// 투사체 비활성화
	UFUNCTION()
	void DeactivateProjectile();

protected:
	// 틱 활성화시 실행될 함수
	void ActivateTick();

	// 투사체 활성화
	void ActivateProjectile();

	// 투사체 활성화 여부
	inline bool IsActiveProjectile() { return bApplyDamageActive; }

	// 플레이어와 Overlap 되었을 때
	UFUNCTION()
	void OnPlayerBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	// 플레이어와 Overlap 끝났을 때
	UFUNCTION()	
	void OnPlayerEndOverlap(AActor* OverlappedActor, AActor* OtherActor);

	// 플레이어에게 데미지를 주는 함수
	void ApplyDamageToPlayer(AActor* Target);

	// 투살체의 이동 함수
	void MoveProjectile();

	// 투사체의 초기 세팅 함수
	void InitProjectile();


protected:
	// 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	TObjectPtr<class UArrowComponent> ArrowComponent = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	TObjectPtr<class UCapsuleComponent> CapsuleComponent = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	TObjectPtr<class UStaticMeshComponent> MeshComponent = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara")
	TObjectPtr<class UNiagaraComponent> Niagara = nullptr;

	// 투사체 정보
	// 데미지 배율
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setting|Damage", meta = (ClampMin = "0"))
	float Damagemagnification = 1.0f;
	// 히트시 투사체 비활성화 여부
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setting|Damage")
	bool SingleHitMode = false;
	// 투사체 틱 레이트
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setting|Tick", meta = (ClampMin = "0"))
	float TickRate = 0.0f;
	// 틱당 데미지 활성화
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setting|Tick")
	bool bActiveTickDamage = false;
	// Tick 단위 투사체 이동 거리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setting|Move", meta = (ClampMin = "0"))
	float TickMoveDistance = 0.0f;
	// 투사체 유도 활성화
	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setting|Move")
	float bActiveHoming = false;*/
	// 투사체 지속시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setting|Life", meta = (ClampMin = "0"))
	float LifeTime = 0.0f;

private:
	// 보스
	TWeakObjectPtr<ALastBossCharacter> LastBoss = nullptr;

	// 투사체가 활성화 되었는지에대한 여부
	UPROPERTY(VisibleAnywhere, Category = "bDamageActive")
	bool bApplyDamageActive = false;

	// 투사체의 지속시간 타이머핸들
	FTimerHandle LifeTimerHandle;
	FTimerHandle TickTimerHandle;

	UPROPERTY()
	TSet<TWeakObjectPtr<AActor>> OverlappingTargets;
};
