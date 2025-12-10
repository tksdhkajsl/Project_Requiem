// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BossBase.generated.h"

UENUM(BlueprintType)
enum class EBossState : uint8
{
	Idle			UMETA(DisplayName = "Idle"),
	Chase			UMETA(DisplayName = "Chase"),
	Attack			UMETA(DisplayName = "Attack"),
	PhaseChange 	UMETA(DisplayName = "PhaseChange"),
	Dead        	UMETA(DisplayName = "Dead"),
};

UCLASS()
class PROJECT_REQUIEM_API ABossBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABossBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 보스 상태
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|State")
	EBossState CurrentState = EBossState::Idle;

	// 전투 시작하자마자 자동으로 Chase 상태로 갈지 여부
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|State")
	bool bAutoStartChase = true;

	// 상태 전환용 함수
	void SetBossState(EBossState NewState);

	// 상태별 업데이트 함수
	void UpdateState(float DeltaTime);
	void UpdateIdle(float DeltaTime);
	void UpdateChase(float DeltaTime);
	void UpdateAttack(float DeltaTime);
	void UpdatePhaseChange(float DeltaTime);
	void UpdateDead(float DeltaTime);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// 추적할 타겟
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Target")
	ACharacter* TargetCharacter = nullptr;
	// 보스 이동 속도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Movement")
	float WalkSpeed = 400.0f;

	// 플레이어랑 겹치는거 방지용 최소 거리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Movement")
	float StoppingDistance = 80.0f;




};
