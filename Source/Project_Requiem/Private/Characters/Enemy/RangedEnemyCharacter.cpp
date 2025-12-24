// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Enemy/RangedEnemyCharacter.h"
#include "Stats/StatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/Enemy/EnemyProjectile.h"

ARangedEnemyCharacter::ARangedEnemyCharacter()
{
	// 원거리 적 설정
	AttackRange = 1000.0f;
	DetectionRange = 1500.0f;
	AttackDelay = 2.0f;
	MonsterName = FText::FromString(TEXT("EnemyMage"));
}

void ARangedEnemyCharacter::AttackHitCheck()
{
	// Super::AttackHitCheck() 호출 금지 (근접 공격 판정 안 함)

	if (!ProjectileClass) return;

	// 1. 위치 및 회전 계산
	FVector SpawnLocation = GetActorLocation();
	FRotator SpawnRotation = GetActorRotation();

	if (GetMesh()->DoesSocketExist(MuzzleSocketName))
	{
		SpawnLocation = GetMesh()->GetSocketLocation(MuzzleSocketName);
	
	}

	// 2. 발사체 스폰
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;

	AEnemyProjectile* Projectile = GetWorld()->SpawnActor<AEnemyProjectile>(
		ProjectileClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	);

	// 3. 데미지 주입 및 유도 타겟 설정
	if (Projectile && StatComponent)
	{
		// 공격력 전달
		Projectile->Damage = StatComponent->GetStatCurrent(EFullStats::PhysicalAttack);

		// [핵심] 플레이어를 찾아 유도 타겟으로 지정
		ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
		if (Player)
		{
			Projectile->SetHomingTarget(Player);
		}
	}
}
