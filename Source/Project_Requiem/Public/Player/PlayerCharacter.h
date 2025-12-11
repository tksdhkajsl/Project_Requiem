// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "PlayerCharacter.generated.h"

class UInputConfig;
struct FInputActionValue;

UCLASS()
class PROJECT_REQUIEM_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

#pragma region Input
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInputConfig* InputConfig;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void ViewStat();
#pragma endregion
};
