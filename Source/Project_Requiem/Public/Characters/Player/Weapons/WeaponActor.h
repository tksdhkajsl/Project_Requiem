// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon/WeaponCodeEnum.h"
#include "WeaponActor.generated.h"

class APlayerCharacter;

UCLASS()
class PROJECT_REQUIEM_API AWeaponActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponActor();



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:
	// 메시 위치를 조정하는 내부 함수
	void AdjustMeshToSocket(USkeletalMeshComponent* InMesh);

	// 공격 활성화/비활성화 (플레이어가 호출)
	UFUNCTION(BlueprintCallable)
	void AttackEnable(bool bEnable);

	// 랭크에 맞춰 이펙트 교체 함수
	void UpdateVFXByRank(EWeaponRank NewRank);

protected:

	// [수정] 컴포넌트 오버랩 이벤트에 맞는 시그니처로 변경
	UFUNCTION()
	void OnWeaponBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex,
		bool bFromSweep, 
		const FHitResult& SweepResult);
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
	TObjectPtr<class UNiagaraComponent> WeaponSlashEffectLeft = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UAudioComponent> WeaponAudioComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	EWeaponCode WeaponID = EWeaponCode::OneHandedSword;

	// 에디터에서 "손잡이"라고 적어줄 변수
	UPROPERTY(EditDefaultsOnly, Category = "Socket")
	FName GripSocketName;

	// 랭크별 이펙트 목록
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
	TMap<EWeaponRank, class UNiagaraSystem*> RankVFXMap;

	// [추가] 왼손 소켓 이름 저장용
	UPROPERTY(EditDefaultsOnly, Category = "Socket")
	FName LeftHandGripSocketName = TEXT("HandGripL");

	// [추가] 중복 타격 방지를 위한 목록
	UPROPERTY()
	TArray<AActor*> HitActors;

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
