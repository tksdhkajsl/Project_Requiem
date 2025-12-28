// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Enemy/EnemyCharacter.h"
#include "RangedEnemyCharacter.generated.h"

class AEnemyProjectile;

/**
 * 
 */
UCLASS()
class PROJECT_REQUIEM_API ARangedEnemyCharacter : public AEnemyCharacter
{
	GENERATED_BODY()

public:
	ARangedEnemyCharacter();

protected:
	// 근접 공격 대신 발사체 공격으로 덮어쓰기
	virtual void AttackHitCheck() override;

public:
	// 사용할 발사체 클래스 (BP_EnemyArrow 등)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TSubclassOf<AEnemyProjectile> ProjectileClass;

	// 발사 위치 소켓 (활 끝부분)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	FName MuzzleSocketName = FName("Muzzle_01");
};