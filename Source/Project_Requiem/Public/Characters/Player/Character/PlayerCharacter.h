// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "PlayerCharacter.generated.h"

class UInputConfig;
struct FInputActionValue;
class USceneCaptureComponent2D;

UCLASS()
class PROJECT_REQUIEM_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()
	
public:
	APlayerCharacter();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
protected:
	virtual void BeginPlay() override;

#pragma region 인터렉션 관련
public:
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bEnableRayTrace = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float InteractionTraceLength = 200;
private:
	UPROPERTY()
	AActor* InteractionActor = nullptr;
	// 인터렉션 용
	void TraceForInteraction();
	void UpdateInteractionUI();
	void InputInteract();
#pragma endregion


#pragma region Input
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInputConfig* InputConfig;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Roll(const FInputActionValue& Value);
	void SetSprintMode(const FInputActionValue& Value);
	void SetWalkMode(const FInputActionValue& Value);
	void EquipWeapon(const FInputActionValue& Value);
	void ViewStat();

#pragma endregion

#pragma region 캡쳐 컴포넌트 관련
public:
	// 캡처 컴포넌트를 외부에 노출하여 UMG에서 접근할 수 있도록 합니다.
	FORCEINLINE USceneCaptureComponent2D* GetPortraitCaptureComponent() const { return PortraitCaptureComponent; }
protected:
	/** @brief 플레이어 초상화 생성을 위한 3D 캡처 카메라 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portrait")
	TObjectPtr<USceneCaptureComponent2D> PortraitCaptureComponent;
#pragma endregion

public:
	inline void SetInvincible(bool bNewState) { bIsInvincible = bNewState; }

	bool GetInvincible() const { return bIsInvincible; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|Camera")
	TObjectPtr<class USpringArmComponent> SpringArm = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|Camera")
	TObjectPtr<class UCameraComponent> PlayerCamera = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|Weapon")
	TObjectPtr<class UWeaponManagerComponent> WeaponManager = nullptr;

	// 구르기 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation|Montage")
	TObjectPtr<UAnimMontage> RollMontage = nullptr;

	// 현재 손에 들고 있는 무기
	UPROPERTY(VisibleInstanceOnly, Category = "Combat")
	TObjectPtr<class AWeaponActor> CurrentWeapon = nullptr;

	// 달리기 속도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Movement")
	float SprintSpeed = 1200.0f;
	// 걷기 속도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Movement")
	float WalkSpeed = 600.0f;

	// 플레이어가 뛰고 있는 중인지 표시 해놓은 변수
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player|State")
	bool bIsSprint = false;

	// 실제 무적 상태를 저장할 변수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|State")
	bool bIsInvincible = false;

private:
	UPROPERTY()
	TWeakObjectPtr<UAnimInstance> AnimInstance = nullptr;
};
