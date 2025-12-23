// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UAudioComponent;
class USoundBase;

UCLASS()
class PROJECT_REQUIEM_API AEnemyProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// =============================================================
		// 1. 컴포넌트 (충돌, 이동, 소리)
		// =============================================================
	UPROPERTY(VisibleAnywhere, Category = "Projectile")
	USphereComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, Category = "Movement")
	UProjectileMovementComponent* ProjectileMovement;

	// 날아가는 동안 나는 소리 (웅~)
	UPROPERTY(VisibleAnywhere, Category = "Audio")
	UAudioComponent* FlightAudioComponent;

	// =============================================================
	// 2. 설정 변수 (데미지, 타격음)
	// =============================================================
	float Damage = 10.0f;

	// 무언가에 맞았을 때 재생할 소리 (쾅!)
	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundBase* ImpactSound;

	// =============================================================
	// 3. 유도(Homing) 시스템
	// =============================================================
	// 우리가 직접 추적할 타겟
	UPROPERTY()
	AActor* TargetActor;

	// 유도 시작 함수
	void SetHomingTarget(AActor* Target);

	// 매 프레임 유도 로직 수행
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Homing")
	float InterpSpeed = 1.5f;

	// =============================================================
	// 4. 충돌 이벤트
	// =============================================================
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

};
