#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/Characters/InteractionInterface.h" //
#include "Characters/Player/Character/PlayerCharacter.h"
#include "Bonfire.generated.h"

class UBoxComponent;
class UUserWidget; // 위젯 생성을 위해 전방 선언

UCLASS()
class PROJECT_REQUIEM_API ABonfire : public AActor, public IInteractionInterface
{
	GENERATED_BODY()

public:
	ABonfire();

protected:
	// 상호작용 범위 충돌체
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* InteractionBox;

	// 화톳불 메쉬
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Mesh;

	// [추가] 화톳불 휴식 시 띄울 위젯 클래스 (BP_BonfireUI 같은 거)
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> BonfireWidgetClass;

public:
	// =============================================================
	// IInteractionInterface 구현
	// =============================================================
	virtual bool CanInteract_Implementation(const APlayerCharacter* Caller) const override;
	virtual FText GetInteractionText_Implementation(const APlayerCharacter* Caller) const override;
	virtual void Interact_Implementation(APlayerCharacter* Caller) override;

private:
	// 모든 적 리스폰 로직
	void RespawnAllEnemies();

	// 플레이어 회복 로직
	void HealPlayer(APlayerCharacter* Player);
};