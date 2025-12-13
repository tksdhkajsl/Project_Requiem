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

	// 3. 숫자 -> Enum 변환
	EWeaponCode TargetCode = EWeaponCode::OneHandedSword;

	// 4. 내 컨트롤러 가져오기 (APRPlayerController로 형변환)
	APRPlayerController* PC = Cast<APRPlayerController>(GetController());

	// 5. 입력 번호에 따라 TargetCode 설정 및 UI 갱신
	switch (WeaponIndex)
	{
	case 1:
		TargetCode = EWeaponCode::OneHandedSword; // 1번 -> 한손검
		if (PC)
		{
			PC->PushDownKeyboard1();          // UI 업데이트
		}
		break;

	case 2:
		TargetCode = EWeaponCode::TwoHandedSword; // 2번 -> 양손검
		if (PC)
		{
			PC->PushDownKeyboard2();
		}
		break;

	case 3:
		TargetCode = EWeaponCode::DualBlade;      // 3번 -> 쌍검
		if (PC)
		{
			PC->PushDownKeyboard3();
		}
		break;

	default:
		return; // 1, 2, 3번이 아니면 아무것도 안 함
	}

	// =============================================================
	// 6. 실제 무기 교체 로직 (WeaponManager 활용)
	// =============================================================
	if (WeaponManager)
	{
		// A. 매니저에게서 해당 코드의 무기(미리 소환된 것)를 가져옵니다.
		AWeaponActor* NewWeapon = WeaponManager->GetWeaponInstance(TargetCode);

		// 무기가 존재하고, 현재 들고 있는 무기와 다를 때만 교체 진행
		if (NewWeapon && NewWeapon != CurrentWeapon)
		{
			// B. [기존 무기 정리] 현재 들고 있는 게 있다면 숨깁니다.
			if (CurrentWeapon)
			{
				CurrentWeapon->SetActorHiddenInGame(true);     // 숨김
				CurrentWeapon->SetActorEnableCollision(false); // 충돌 끔

				// 만약 쌍검이라서 왼손 무기가 켜져 있었다면, 그것도 끕니다.
				if (auto* LeftMesh = CurrentWeapon->GetLeftWeaponMesh())
				{
					LeftMesh->SetVisibility(false);
				}
			}

			// C. [새 무기 장착] 숨겨져 있던 무기를 꺼냅니다.
			NewWeapon->SetActorHiddenInGame(false);   // 보임
			NewWeapon->SetActorEnableCollision(true); // 충돌 켬

			// D. 오른손 소켓에 붙입니다. (공통)
			NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("hand_rSocket"));

			// E. [쌍검 체크] 왼손 무기가 있는 경우 처리
			if (auto* LeftMesh = NewWeapon->GetLeftWeaponMesh())
			{
				// 1. 보이게 하고 소켓에 붙임 (여기서 SnapToTarget 때문에 위치가 0,0,0으로 초기화됨!)
				LeftMesh->SetVisibility(true);
				LeftMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("hand_lSocket"));

				// 2. [추가] 초기화된 위치를 다시 "손잡이 기준"으로 재조정!
				NewWeapon->AdjustMeshToSocket(LeftMesh);
			}

			// F. 현재 무기 변수 갱신 ("이제 이게 내 무기다!")
			CurrentWeapon = NewWeapon;
		}
	}
}
void APlayerCharacter::ViewStat()
{
	//if (APrimordiaPlayerController* PC = GetController<APrimordiaPlayerController>()) {
	//	PC->RequestStatView(this);
	//}
}