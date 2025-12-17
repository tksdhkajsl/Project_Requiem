// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Player/Character/PlayerAnimInstance.h"
#include "Characters/Player/Character/PlayerCharacter.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Animation/BlendSpace.h"

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	APawn* ownerPawn = TryGetPawnOwner();
	if (ownerPawn)
	{
		OwnerMovementComponent = ownerPawn->GetMovementComponent();
	}
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (OwnerMovementComponent.IsValid())
	{
		Speed = OwnerMovementComponent.Get()->Velocity.Size();
	}

    if (OwnerCharacter.IsValid())
    {
        EWeaponCode CurrentWeapon = OwnerCharacter->GetCurrentWeaponType();

        switch (CurrentWeapon)
        {
        case EWeaponCode::TwoHandedSword:
            // [양손검]만 전용 블렌드 스페이스 사용
            if (TwoHandedLocomotionBS)
            {
                CurrentLocomotionBS = TwoHandedLocomotionBS;
            }
            break;

        case EWeaponCode::OneHandedSword:
        case EWeaponCode::DualBlade:
        default:
            // [한손검]과 [쌍검], 그리고 [나머지]는 모두 공용(Normal) 사용!
            if (NormalLocomotionBS)
            {
                CurrentLocomotionBS = NormalLocomotionBS;
            }
            break;
        }
    }
}
