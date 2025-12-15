// BossProjectile.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class PROJECT_REQUIEM_API ABossProjectile : public AActor
{
	GENERATED_BODY()

public:
	ABossProjectile();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

protected:
	// 충돌 구체(루트)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile|Component")
	USphereComponent* Collision = nullptr;

	// 투사체 이동 담당(속도/중력/회전 등)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile|Component")
	UProjectileMovementComponent* ProjectileMovement = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile|Damage")
	float Damage = 20.0f; // 기본 데미지(InitProjectile에서 덮어씀)

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile|Life")
	float LifeTime = 5.0f; // 몇 초 후 자동 파괴(SetLifeSpan)

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile|State")
	bool bHitOnce = false; // 한번 맞히면 추가 히트 방지

public:
	// 보스가 스폰 직후 호출해서 데미지/인스티게이터 설정
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void InitProjectile(float InDamage, AController* InInstigatorController);

protected:
	// Pawn과 Overlap 시(플레이어 맞추기)
	UFUNCTION()
	void OnCollisionBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	// 벽/바닥 등 Block 충돌 시
	UFUNCTION()
	void OnCollisionHit(
		UPrimitiveComponent* HitComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit
	);

protected:
	// 공통 처리: 데미지 적용 + 파괴
	void HandleImpactAndDestroy(AActor* HitActor);
};
