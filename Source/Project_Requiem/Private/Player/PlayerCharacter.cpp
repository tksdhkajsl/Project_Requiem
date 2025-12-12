#include "Player/PlayerCharacter.h"

#pragma region Input
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Weapons/WeaponManagerComponent.h"
#include "Core/PRPlayerController.h"
#include "Config/InputConfig.h"
#pragma endregion

APlayerCharacter::APlayerCharacter()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 350.0f;
	SpringArm->SocketOffset = FVector(0, 0, 250);
	SpringArm->bUsePawnControlRotation = true;	// 스프링암의 회전을 컨트롤러에 맞춤

	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	PlayerCamera->SetupAttachment(SpringArm);
	PlayerCamera->SetRelativeRotation(FRotator(-20.0f, 0.0f, 0.0f));

	bUseControllerRotationYaw = false;	// 컨트롤러의 Yaw 회전 사용 안함
	GetCharacterMovement()->bOrientRotationToMovement = true;	// 이동 방향으로 캐릭터 회전
	GetCharacterMovement()->RotationRate = FRotator(0, 360, 0);

	WeaponManager = CreateDefaultSubobject<UWeaponManagerComponent>(TEXT("WeaponManager"));
}

// ========================================================
// 언리얼 기본 생성 및 초기화
// ========================================================
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (APRPlayerController* PlayerController = Cast<APRPlayerController>(GetController())) {
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) {
			Subsystem->AddMappingContext(InputConfig->DefaultMappingContext, 0);
		}
	}

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (EIC) 
	{
		// 기본 이동 입력
		UInputAction* MoveAction = InputConfig->GetAction(EHumanoidInput::Move);
		if (MoveAction) EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);

		// 화면 회전 입력
		UInputAction* LookAction = InputConfig->GetAction(EHumanoidInput::Look);
		if (LookAction) EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);

		// 구르기(회피) 입력
		UInputAction* RollAction = InputConfig->GetAction(EHumanoidInput::Roll);
		if (RollAction) EIC->BindAction(RollAction, ETriggerEvent::Triggered, this, &ThisClass::Roll);

		UInputAction* SprintAction = InputConfig->GetAction(EHumanoidInput::Sprint);
		if (SprintAction)
		{
			// 눌렀을 때 (Started) -> 달리기 모드
			EIC->BindAction(SprintAction, ETriggerEvent::Started, this, &APlayerCharacter::SetSprintMode);

			// 손을 뗐을 때 (Completed) -> 걷기 모드
			EIC->BindAction(SprintAction, ETriggerEvent::Completed, this, &APlayerCharacter::SetWalkMode);
		}

		// 무기 교체 입력 바인딩 (1, 2, 3번 키 통합)
		UInputAction* EquipAction = InputConfig->GetAction(EHumanoidInput::EquipWeapon);
		if (EquipAction)
		{
			// Triggered로 설정하면 키를 누르는 순간 실행됩니다.
			EIC->BindAction(EquipAction, ETriggerEvent::Triggered, this, &ThisClass::EquipWeapon);
		}

		UInputAction* StatAction = InputConfig->GetAction(EHumanoidInput::Stat);
		if (StatAction) EIC->BindAction(StatAction, ETriggerEvent::Completed, this, &ThisClass::ViewStat);
	}
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GetMesh())
	{
		AnimInstance = GetMesh()->GetAnimInstance();	// ABP 객체 가져오기		
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

	if (Controller != nullptr) 
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);

		////민감도 적용버전
		//AddControllerYawInput(LookAxisVector.X * BaseTurnRate * GetWorld()->GetDeltaSeconds()); // 이 경우 BaseTurnRate는 스케일이 아닌 'Rate'로 사용됨
		//AddControllerPitchInput(LookAxisVector.Y * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
	}
}
void APlayerCharacter::Roll(const FInputActionValue& Value)
{
	if (AnimInstance.IsValid() && !GetController()->IsMoveInputIgnored())
	{

		// && Resource->HasEnoughStamina(RollStaminaCost)
		if (!AnimInstance->IsAnyMontagePlaying())	// 몽타주 재생중이 아니면 작동
		{
			if (!GetLastMovementInputVector().IsNearlyZero())	// 입력을 하는 중에만 즉시 회전
			{
				SetActorRotation(GetLastMovementInputVector().Rotation());	// 마지막 입력 방향으로 즉시 회전 시키기
			}
			//Resource->AddStamina(-RollStaminaCost);	// 스태미너 감소
			PlayAnimMontage(RollMontage);
		}
	}
}
void APlayerCharacter::SetSprintMode(const FInputActionValue& Value)
{
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	bIsSprint = true;
}
void APlayerCharacter::SetWalkMode(const FInputActionValue& Value)
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	bIsSprint = false;
}
void APlayerCharacter::EquipWeapon(const FInputActionValue& Value)
{
	// 1. 입력값 가져오기 (IMC 스칼라 값: 1.0, 2.0, 3.0)
	float InputValue = Value.Get<float>();

	// 2. 정수로 변환 (1, 2, 3)
	int32 WeaponIndex = (int32)InputValue;



	// 3. 내 컨트롤러 가져오기 (APRPlayerController로 형변환)
	APRPlayerController* PC = Cast<APRPlayerController>(GetController());

	if (PC)
	{
		switch (WeaponIndex)
		{
		case 1:
			// 1번 키: 한손검
			// TODO: 실제 캐릭터의 무기 교체 로직 (예: 메시 변경)은 여기에 작성
			// EquipOneHandedSword(); 

			// HUD 업데이트 (컨트롤러 함수 호출)
			PC->PushDownKeyboard1();
			break;

		case 2:
			// 2번 키: 양손검
			// EquipTwoHandedSword();

			// HUD 업데이트
			PC->PushDownKeyboard2();
			break;

		case 3:
			// 3번 키: 쌍검
			// EquipDualBlade();

			// HUD 업데이트
			PC->PushDownKeyboard3();
			break;

		default:
			break;
		}
	}
}
void APlayerCharacter::ViewStat()
{
	//if (APrimordiaPlayerController* PC = GetController<APrimordiaPlayerController>()) {
	//	PC->RequestStatView(this);
	//}
}