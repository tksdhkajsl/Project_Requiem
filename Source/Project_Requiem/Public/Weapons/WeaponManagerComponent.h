// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Common/CommonEnum.h"
#include "Weapons/WeaponActor.h"
#include "Data/WeaponDataAsset.h"
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
	// [에디터 설정용] 무기 코드별 데이터 에셋 (블루프린트 클래스 정보 포함)
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Database")
	TMap<EWeaponCode, TObjectPtr<UWeaponDataAsset>> WeaponDatabase;

	// [실제 게임용] 생성된 무기 액터들을 보관하는 창고
	UPROPERTY(VisibleInstanceOnly, Category = "Weapon Instance")
	TMap<EWeaponCode, TObjectPtr<AWeaponActor>> WeaponInstances;
		
};
