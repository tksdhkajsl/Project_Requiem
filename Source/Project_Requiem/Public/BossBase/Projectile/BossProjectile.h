// BossProjectile.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ShapeComponent.h"
#include "BossProjectile.generated.h"

class UShapeComponent;
class USphereComponent;
class UCapsuleComponent;

class UProjectileMovementComponent;

class UNiagaraComponent;
class UNiagaraSystem;

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
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile|Component")
	UShapeComponent* Collision = nullptr;

	// 캡슐 콜리전
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile|Component")
	UCapsuleComponent* CapsuleCollision = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile|Component")
	UProjectileMovementComponent* ProjectileMovement = nullptr;

	// 캡슐 콜리전을 사용할지 말지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile|Component")
	bool bUseCapsuleCollision = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile|Component")
	UPrimitiveComponent* ActiveCollision = nullptr;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile|Damage")
	float Damage = 20.0f; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile|Life")
	float LifeTime = 5.0f; 

	// Pawn에 맞으면 사라질지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile|Behavior")
	bool bDestroyOnPawnHit = true;

	// 월드에 맞으면 사라질지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile|Behavior")
	bool bDestroyOnWorldHit = true;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile|State")
	bool bHitOnce = false; 

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile|VFX")
	UNiagaraComponent* VfxComp = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile|VFX")
	UNiagaraSystem* VfxSystem = nullptr;

public:
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void InitProjectile(float InDamage, AController* InInstigatorController);

protected:
	UFUNCTION()
	void OnCollisionBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnCollisionHit(
		UPrimitiveComponent* HitComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit
	);

protected:
	void HandleImpactAndDestroy(AActor* HitActor);
};
