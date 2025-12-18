// BossProjectile.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossProjectile.generated.h"

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
	UCapsuleComponent* Collision = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile|Component")
	UProjectileMovementComponent* ProjectileMovement = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile|Damage")
	float Damage = 20.0f; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile|Life")
	float LifeTime = 5.0f; 

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
