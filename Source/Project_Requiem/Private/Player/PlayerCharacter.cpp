#include "Player/PlayerCharacter.h"

#pragma region Input
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Config/InputConfig.h"
#pragma endregion

// ========================================================
// 언리얼 기본 생성 및 초기화
// ========================================================
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController())) {
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) {
			Subsystem->AddMappingContext(InputConfig->DefaultMappingContext, 0);
		}
	}

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		UInputAction* MoveAction = InputConfig->GetAction(EHumanoidInput::Move);
		if (MoveAction) EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);

		UInputAction* LookAction = InputConfig->GetAction(EHumanoidInput::Look);
		if (LookAction) EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);

		UInputAction* StatAction = InputConfig->GetAction(EHumanoidInput::Stat);
		if (StatAction) EIC->BindAction(StatAction, ETriggerEvent::Completed, this, &ThisClass::ViewStat);
	}
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr) {
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}
void APlayerCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr) {
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);

		////민감도 적용버전
		//AddControllerYawInput(LookAxisVector.X * BaseTurnRate * GetWorld()->GetDeltaSeconds()); // 이 경우 BaseTurnRate는 스케일이 아닌 'Rate'로 사용됨
		//AddControllerPitchInput(LookAxisVector.Y * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
	}
}
void APlayerCharacter::ViewStat()
{
	//if (APrimordiaPlayerController* PC = GetController<APrimordiaPlayerController>()) {
	//	PC->RequestStatView(this);
	//}
}