// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Weapon/WeaponCodeEnum.h"
#include "PlayerAnimInstance.generated.h"

class UBlendSpace;
class APlayerCharacter;
class UPawnMovementComponent;

/**
 * 
 */
UCLASS()
class PROJECT_REQUIEM_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	float Speed = 0.0f;

	// 1. 기본 이동 (한손검 + 쌍검 공용)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Locomotion|Assets")
	TObjectPtr<UBlendSpace> NormalLocomotionBS = nullptr; // 이름 변경: OneHanded -> Normal

	// 2. 양손검 이동 (양손검 전용)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Locomotion|Assets")
	TObjectPtr<UBlendSpace> TwoHandedLocomotionBS = nullptr;

	// 현재 적용 중인 BS
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Locomotion")
	TObjectPtr<UBlendSpace> CurrentLocomotionBS = nullptr;

private:
	TWeakObjectPtr<const UPawnMovementComponent> OwnerMovementComponent = nullptr;

	TWeakObjectPtr<APlayerCharacter> OwnerCharacter = nullptr;
};
