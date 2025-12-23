#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/Characters/InteractionInterface.h"
#include "Characters/Player/Character/PlayerCharacter.h"
#include "Bonfire.generated.h"

class UBoxComponent;
class UNiagaraComponent;
class UNiagaraSystem;
class UBonfireWidget;

UENUM(BlueprintType)
enum class EInteractionType : uint8
{
	None,
	Bonfire,
	Merchant,
	Door,
	Pickup
};

UCLASS()
class PROJECT_REQUIEM_API ABonfire : public AActor, public IInteractionInterface
{
	GENERATED_BODY()

#pragma region 언리얼 생성
public:
	ABonfire();
protected:
	virtual void BeginPlay() override;
#pragma endregion

public:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UBonfireWidget> BonfireWidgetClass;
	UPROPERTY()
	TObjectPtr<UBonfireWidget> BonfireWidget;


protected:
	// 상호작용 범위 충돌체
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* InteractionBox;

	// 화톳불 메쉬
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Mesh;

	/* 
	*  25/12/17 코드 추가 : 천수호
	*  추가 사유 : 나이아가라 이펙트 추가
	*/
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UNiagaraComponent* FireEffect;
	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	UNiagaraSystem* FireSystem;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite,Category = "00_Settings")
	EInteractionType InteractionType = EInteractionType::Bonfire;

public:
	// =============================================================
	// IInteractionInterface 구현
	// =============================================================
#pragma region Interaction Interface
public:
	virtual bool CanInteract_Implementation(const APlayerCharacter* Caller) const override;
	virtual FText GetInteractionText_Implementation(const APlayerCharacter* Caller) const override;
	virtual void Interact_Implementation(APlayerCharacter* Caller) override;
	UFUNCTION()
	void HandleWidgetClosed();
private:
	bool bIsOpened = false;
#pragma endregion

private:
	// 모든 적 리스폰 로직
	void RespawnAllEnemies();

	// 플레이어 회복 로직
	void HealPlayer(AActor* PlayerActor);

};