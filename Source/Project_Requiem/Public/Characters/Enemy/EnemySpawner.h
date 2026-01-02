// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/Characters/InteractionInterface.h"
#include "Characters/Player/Character/PlayerCharacter.h"
#include "EnemySpawner.generated.h"

class AEnemyCharacter;

UCLASS()
class PROJECT_REQUIEM_API AEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:
	// =============================================================
	// 설정 변수
	// =============================================================
	/** 이 위치에 소환할 적 클래스 (BP_Enemy 지정) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner")
	TSubclassOf<AEnemyCharacter> EnemyClass;

	/** 현재 관리 중인 적 인스턴스 (메모리 관리용) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawner")
	AEnemyCharacter* SpawnedEnemy;

	// =============================================================
	// 기능 함수
	// =============================================================
	/** * 적을 리스폰시키는 함수.
	 * - 적이 없으면 새로 생성 (Spawn)
	 * - 적이 있으면 상태 초기화 (Reset)
	 * - 화톳불(Bonfire)에서 호출됨
	 */
	UFUNCTION(BlueprintCallable, Category = "Spawner")
	void RespawnEnemy();




};
