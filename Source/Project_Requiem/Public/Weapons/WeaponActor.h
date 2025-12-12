// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Player/PlayerCharacter.h"
#include "Common/CommonEnum.h"
#include "WeaponActor.generated.h"

UCLASS()
class PROJECT_REQUIEM_API AWeaponActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponActor();

	// 메시 위치를 조정하는 내부 함수
	void AdjustMeshToSocket(USkeletalMeshComponent* InMesh);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnWeaponBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	virtual void OnWeaponActivate() {};
	virtual void OnWeaponDeactivate() {};

public:	
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	FORCEINLINE USkeletalMeshComponent* GetLeftWeaponMesh() const { return LeftWeaponMesh; }

protected:
	// =================================================
	//  오른손 (Main)
	// =================================================
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UBoxComponent> WeaponCollision;

	// =================================================
	//  왼손 (Sub / Dual Wielding)
	// =================================================
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> LeftWeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UBoxComponent> LeftWeaponCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UNiagaraComponent> WeaponSlashEffect = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UAudioComponent> WeaponAudioComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	EWeaponCode WeaponID = EWeaponCode::OneHandedSword;

	// 에디터에서 "손잡이"라고 적어줄 변수
	UPROPERTY(EditDefaultsOnly, Category = "Socket")
	FName GripSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	float Damage = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	TSubclassOf<UDamageType> DamageType = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data|Area")
	float AreaInnerRadius = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data|Area")
	float AreaOuterRadius = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data|Area")
	float Falloff = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data|Area")
	float DebugDuration = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data|Area")
	TObjectPtr<class UNiagaraSystem> AreaAttackEffect = nullptr;

private:
	TWeakObjectPtr<APlayerCharacter> WeaponOwner = nullptr;

};
