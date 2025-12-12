// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/WeaponManagerComponent.h"

// Sets default values for this component's properties
UWeaponManagerComponent::UWeaponManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UWeaponManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// 게임 시작 시 무기들을 미리 다 만들어둡니다.
	SpawnWeaponInstances();
}

AWeaponActor* UWeaponManagerComponent::GetWeaponInstance(EWeaponCode InCode) const
{
	// 맵에 해당 코드가 있는지 확인하고 반환
	if (WeaponInstances.Contains(InCode))
	{
		return WeaponInstances[InCode];
	}

	return nullptr; // 없으면 null 반환
}

void UWeaponManagerComponent::SpawnWeaponInstances()
{
	// 주인이 없거나 월드가 없으면 중단
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter || !GetWorld()) return;

	// 데이터베이스에 등록된 모든 무기를 순회하며 생성
	for (const auto& Pair : WeaponDatabase)
	{
		EWeaponCode Code = Pair.Key;
		UWeaponDataAsset* Data = Pair.Value;

		if (Data && Data->EquippedWeaponClass)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = OwnerCharacter;
			SpawnParams.Instigator = OwnerCharacter;

			// 1. 무기 소환 (위치는 일단 캐릭터 위치)
			AWeaponActor* NewWeapon = GetWorld()->SpawnActor<AWeaponActor>(
				Data->EquippedWeaponClass,
				OwnerCharacter->GetActorLocation(),
				OwnerCharacter->GetActorRotation(),
				SpawnParams
			);

			if (NewWeapon)
			{
				// 2. 소환하자마자 일단 숨김 (장착하기 전까지는 안 보여야 함)
				NewWeapon->SetActorHiddenInGame(true);
				NewWeapon->SetActorEnableCollision(false); // 충돌도 끔
				NewWeapon->AttachToComponent(OwnerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("root")); // 임시로 붙여둠 (안보이게)

				// 3. 인스턴스 맵에 저장 (나중에 꺼내 쓰기 위해)
				WeaponInstances.Add(Code, NewWeapon);
			}
		}
	}
}



