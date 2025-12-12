// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/WeaponManagerComponent.h"
#include "GameFramework/Character.h"

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
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter || !GetWorld() || !WeaponDataTable) return;

	// 1. 데이터 테이블의 모든 행(Row) 가져오기
	FString ContextString; // 에러 로그용 문자열
	TArray<FWeaponTableRow*> AllRows;
	WeaponDataTable->GetAllRows<FWeaponTableRow>(ContextString, AllRows);

	// 2. 테이블에 적힌 모든 무기를 하나씩 생성
	for (FWeaponTableRow* Row : AllRows)
	{
		if (Row && Row->WeaponClass)
		{
			// 중복 생성 방지 (이미 맵에 있으면 패스)
			if (WeaponInstances.Contains(Row->WeaponCode)) continue;

			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = OwnerCharacter;
			SpawnParams.Instigator = OwnerCharacter;

			// 무기 액터 소환!
			AWeaponActor* NewWeapon = GetWorld()->SpawnActor<AWeaponActor>(
				Row->WeaponClass,
				OwnerCharacter->GetActorLocation(),
				OwnerCharacter->GetActorRotation(),
				SpawnParams
			);

			if (NewWeapon)
			{
				// 3. 일단 숨겨두기 (장착 전이니까)
				NewWeapon->SetActorHiddenInGame(true);
				NewWeapon->SetActorEnableCollision(false);
				// 캐릭터 루트에 임시로 붙여둠 (따라다니게)
				NewWeapon->AttachToComponent(OwnerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("root"));

				// 4. 창고(Map)에 저장
				WeaponInstances.Add(Row->WeaponCode, NewWeapon);
			}
		}
	}
}



