// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Common/CommonEnum.h"
#include "Weapons/WeaponActor.h"
#include "Engine/DataTable.h"
#include "Data/WeaponDataTable.h"
#include "WeaponManagerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_REQUIEM_API UWeaponManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeaponManagerComponent();

protected:
	virtual void BeginPlay() override;

public:
	// 특정 코드에 해당하는 '이미 생성된' 무기 액터를 반환하는 함수
	AWeaponActor* GetWeaponInstance(EWeaponCode InCode) const;

private:
	// 시작 시 무기들을 미리 생성해두는 내부 함수
	void SpawnWeaponInstances();

protected:
	// 여기에 아까 만든 데이터 테이블(DT_WeaponData)을 넣을 겁니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Database")
	TObjectPtr<UDataTable> WeaponDataTable;

	// 생성된 무기들을 보관하는 창고 (Key: 무기 종류, Value: 소환된 액터)
	UPROPERTY(VisibleInstanceOnly, Category = "Weapon Instance")
	TMap<EWeaponCode, TObjectPtr<AWeaponActor>> WeaponInstances;
		
};
