// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/Characters/InteractionInterface.h"
#include "Characters/Player/Character/PlayerCharacter.h"
#include "Bonfire.generated.h"

class UBoxComponent;

UCLASS()
class PROJECT_REQUIEM_API ABonfire : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABonfire();

protected:
	// 상호작용 범위 충돌체
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* InteractionBox;

	// 화톳불 메쉬
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Mesh;

public:
	// =============================================================
	// IInteractionInterface 구현
	// =============================================================
#pragma region Interaction Interface
public:
	virtual bool CanInteract_Implementation(const APlayerCharacter* Caller) const override;
	virtual FText GetInteractionText_Implementation(const APlayerCharacter* Caller) const override;
	virtual void Interact_Implementation(APlayerCharacter* Caller) override;
private:
	bool bIsOpened = false;
#pragma endregion

private:
	// 모든 적 리스폰 로직
	void RespawnAllEnemies();

	// 플레이어 회복 로직
	void HealPlayer(AActor* PlayerActor);

};
