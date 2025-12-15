#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/Characters/InteractionInterface.h"
#include "Characters/Player/Character/PlayerCharacter.h"
#include "NpcMerchant.generated.h"

class UBoxComponent;

UCLASS()
class PROJECT_REQUIEM_API ANpcMerchant : public AActor, public IInteractionInterface
{
    GENERATED_BODY()


#pragma region 언리얼 생성
public:
    ANpcMerchant();
protected:
    virtual void BeginPlay() override;
    UPROPERTY(VisibleAnywhere)
    USkeletalMeshComponent* Mesh;
    UPROPERTY(VisibleAnywhere)
    UBoxComponent* InteractionBox;
#pragma endregion



#pragma region Interaction Interface
public:
    virtual bool CanInteract_Implementation(const APlayerCharacter* Caller) const override;
    virtual FText GetInteractionText_Implementation(const APlayerCharacter* Caller) const override;
    virtual void Interact_Implementation(APlayerCharacter* Caller) override;
private:
    bool bIsOpened = false;
#pragma endregion
};