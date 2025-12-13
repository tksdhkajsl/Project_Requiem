#pragma once
#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "InputConfig.generated.h"

UENUM(BlueprintType)
enum class EHumanoidInput : uint8
{
    Move,
    Look,
    Roll,
    Attack,
    HeavyAttack,
    Sprint,
    Inventory,
    Interact,
    Stat,
    EquipWeapon,
};

UCLASS(BlueprintType)
class PROJECT_REQUIEM_API UInputConfig : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UInputMappingContext* DefaultMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TMap<EHumanoidInput, UInputAction*> InputActions;

public:
    UInputAction* GetAction(EHumanoidInput Input) const
    {
        if (InputActions.Contains(Input)) return InputActions[Input];
        return nullptr;
    }
};