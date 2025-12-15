#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractionInterface.generated.h"

UINTERFACE(BlueprintType)
class UInteractionInterface : public UInterface
{
    GENERATED_BODY()
};

class PROJECT_REQUIEM_API IInteractionInterface
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    bool CanInteract(const class APlayerCharacter* Caller) const;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    FText GetInteractionText(const class APlayerCharacter* Caller) const;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void Interact(class APlayerCharacter* Caller);
};