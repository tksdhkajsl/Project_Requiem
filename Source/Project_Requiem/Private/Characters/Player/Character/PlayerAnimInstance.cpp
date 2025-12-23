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
        // 1. 무브먼트 컴포넌트 가져오기
        OwnerMovementComponent = ownerPawn->GetMovementComponent();

        // [중요 수정] 2. 플레이어 캐릭터로 캐스팅해서 변수에 저장! (이게 없으면 아래 로직이 안 돕니다)
        OwnerCharacter = Cast<APlayerCharacter>(ownerPawn);
    }

    // 3. 게임 시작 시 기본 블렌드 스페이스로 초기화 (T-Pose 방지)
    if (NormalLocomotionBS)
    {
        CurrentLocomotionBS = NormalLocomotionBS;
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
                if (CurrentLocomotionBS != TwoHandedLocomotionBS)
                {
                    CurrentLocomotionBS = TwoHandedLocomotionBS;
                }
            }
            break;

        case EWeaponCode::OneHandedSword:
        case EWeaponCode::DualBlade:
        default:
            // [한손검]과 [쌍검], 그리고 [나머지]는 모두 공용(Normal) 사용!
            if (NormalLocomotionBS)
            {
                if (CurrentLocomotionBS != NormalLocomotionBS)
                {
                    CurrentLocomotionBS = NormalLocomotionBS;
                }
            }
            break;
        }
    }
}
