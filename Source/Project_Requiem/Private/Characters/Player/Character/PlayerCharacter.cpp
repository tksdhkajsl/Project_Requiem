#include "Characters/Player/Character/PlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Camera/CameraShakeBase.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Characters/Player/Weapons/WeaponManagerComponent.h"
#include "Characters/Player/Weapons/WeaponMasteryComponent.h"
#include "Characters/Player/Components/LockOnComponent.h"
#include "Components/AudioComponent.h"
#include "Characters/Player/Data/Sounds/SoundDataStruct.h"

#pragma region Input
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Core/PRPlayerController.h"
#include "Characters/Player/Config/InputConfig.h"
#pragma endregion

#include "Components/SceneCaptureComponent2D.h"
#include "Interface/Characters/InteractionInterface.h"

#include "Stats/StatComponent.h"
#include "Components/CapsuleComponent.h"
#include "UI/PlayerDeath/PlayerDeathWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"

#include "UI/HUD/PRHUDWidget.h"
#include "Inventory/InventoryComponent.h"
#include "UI/Inventory/InventoryWidget.h"

#include "Core/PRGameInstance.h"
#include "Mics/Boss/BossDoor.h"

// ========================================================
// 언리얼 기본 생성 및 초기화
// ========================================================
APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bTickEvenWhenPaused = false;

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
	WeaponMastery = CreateDefaultSubobject<UWeaponMasteryComponent>(TEXT("WeaponMastery"));
	LockOnComponent = CreateDefaultSubobject<ULockOnComponent>(TEXT("LockOnComponent"));

	InteractionTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("Interactio Trigger Volume"));
	InteractionTrigger->SetupAttachment(RootComponent);
	InteractionTrigger->SetRelativeScale3D(FVector(10));
	InteractionTrigger->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnInteractionTriggerOverlapBegin);
	InteractionTrigger->OnComponentEndOverlap.AddDynamic(this, &APlayerCharacter::OnInteractionTriggerOverlapEnd);

#pragma region 캡쳐 컴포넌트 관련
	PortraitCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("PortraitCapture"));
	PortraitCaptureComponent->SetupAttachment(RootComponent);
	PortraitCaptureComponent->SetRelativeLocation(FVector(150.f, 0.f, 70.f));
	PortraitCaptureComponent->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));
	PortraitCaptureComponent->bCaptureEveryFrame = false;
	PortraitCaptureComponent->bAlwaysPersistRenderingState = true;
	PortraitCaptureComponent->CaptureSource = ESceneCaptureSource::SCS_SceneColorHDR;
#pragma endregion

	// Stage2용 나이아가라
	SnowNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SnowNiagara"));
	SnowNiagara->SetupAttachment(RootComponent);
	SnowNiagara->SetVariableFloat(TEXT("User.SpawnRateMultiplier"), 1.0f);

	// 포션용 나이아가라 컴포넌트
	PotionEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PotionEffectComponent"));
	PotionEffectComponent->SetupAttachment(RootComponent);
	PotionEffectComponent->bAutoActivate = false; // 게임 시작하자마자 이펙트 나오면 안 되므로 꺼둠

	// [추가] 12/29, 플레이어의 걷기, 달리기 사운드를 관리할 컴포넌트
	MovementAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("MovementAudioComp"));
	MovementAudioComp->SetupAttachment(GetRootComponent());
	MovementAudioComp->bAutoActivate = false; // 평소엔 꺼둠
}
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
		if (MoveAction)
		{
			EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
			
			// [추가] 12/29, 이동 입력에서 손을 뗐을 때 걷거나 달리는 사운드 끔
			EIC->BindAction(MoveAction, ETriggerEvent::Completed, this, &ThisClass::StopMoveSound);
		}

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
		UInputAction* PostionAction = InputConfig->GetAction(EHumanoidInput::EatPotion);
		if (PostionAction)
		{
			// Triggered로 설정하면 키를 누르는 순간 실행됩니다.
			EIC->BindAction(PostionAction, ETriggerEvent::Triggered, this, &ThisClass::EatPotion);
		}

		UInputAction* AttackAction = InputConfig->GetAction(EHumanoidInput::Attack); // Enum 이름 확인 필요
		if (AttackAction)
		{
			EIC->BindAction(AttackAction, ETriggerEvent::Started, this, &ThisClass::AttackInput);
		}

		UInputAction* StatAction = InputConfig->GetAction(EHumanoidInput::Stat);
		if (StatAction) EIC->BindAction(StatAction, ETriggerEvent::Completed, this, &ThisClass::ViewStat);

		UInputAction* InteractAction = InputConfig->GetAction(EHumanoidInput::Interact);
		if (InteractAction) EIC->BindAction(InteractAction, ETriggerEvent::Completed, this, &ThisClass::InputInteract);

		UInputAction* LockOnAction = InputConfig->GetAction(EHumanoidInput::LockOn);
		if (LockOnAction)
		{
			EIC->BindAction(LockOnAction, ETriggerEvent::Started, this, &ThisClass::InputLockOn);
		}
	}
}
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsDeath) return;

	if (bEnableRayTrace) {
		TraceForInteraction();
	}
	// 1. 달리기 모드여야 함 (bIsSprint)
	// 2. 공격 중이 아니어야 함 (!bIsAttacking)
	// 3. 실제로 움직이고 있어야 함 (Velocity > 0) -> 제자리에서 벽에 막히거나 멈췄을 때 감소 방지
	if (bIsSprint && !bIsAttacking && GetVelocity().Size2D() > 1.0f) {
		StatComponent->ChangeStatCurrent(EFullStats::Stamina, -0.1f);

		// 스태미나가 다 떨어지면
		if (StatComponent->GetStatCurrent(EFullStats::Stamina) <= 0.f) {
			// 강제로 걷기 모드로 전환 (빈 값(FInputActionValue)을 넣어 함수 재활용)
			SetWalkMode(FInputActionValue());
		}
	}
	if (CurrentStage == EStageType::Stage2 && CachedBoss) {
		
		if (!SnowNiagara->IsActive()) SnowNiagara->Activate(true);
		float Dist = FVector::Distance(GetActorLocation(), CachedBoss->GetActorLocation());
		float Alpha = 1.f - FMath::Clamp(Dist / MaxDist, 0.f, 1.f);
		float SpawnRate = FMath::Lerp(1.f, 500.f, Alpha);
		SnowNiagara->SetVariableFloat(TEXT("User.SpawnRateMultiplier"), SpawnRate);
	}
	else if (SnowNiagara->IsActive()) SnowNiagara->Deactivate();		
}
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	LoadPlayerComponents();

	//APlayerController* PC = Cast<APlayerController>(GetController());
	//if (PC) {
	//	PC->SetIgnoreMoveInput(false);
	//	PC->SetIgnoreLookInput(false);

	//	FInputModeGameOnly InputMode;
	//	PC->SetInputMode(InputMode);
	//	PC->bShowMouseCursor = false;
	//	PC->SetControlRotation(FRotator::ZeroRotator);
	//}

	// [추가] 12/30, 플레이어 ViewPitch 제한
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		// PlayerCameraManager가 유효한지 확인
		if (PC->PlayerCameraManager)
		{
			// ViewPitchMin: 아래로 내려다보는 제한 (기본값 -90.0)
			PC->PlayerCameraManager->ViewPitchMin = -30.0f;

			// ViewPitchMax: 위로 올려다보는 제한 (기본값 90.0)
			PC->PlayerCameraManager->ViewPitchMax = 45.0f;
		}
	}

	if (GetMesh()) {
		AnimInstance = GetMesh()->GetAnimInstance();	// ABP 객체 가져오기		
	}

	EquipWeapon(1.f);
	// [수정] 12/30, 무기 장착 사운드를 실행해라
	bCanPlayEquipSound = true;
	IsDeath = false;
	SetSpawnLocation(this->GetActorLocation());
	if (!CachedBoss && BossClass) {
		CachedBoss = UGameplayStatics::GetActorOfClass(GetWorld(), BossClass);
	}

#pragma region 캡쳐 컴포넌트 관련
	if (PortraitCaptureComponent) {
		TArray<AActor*> ActorsToShow;
		ActorsToShow.Add(this);
		PortraitCaptureComponent->ShowOnlyActors = ActorsToShow;
		PortraitCaptureComponent->bCaptureEveryFrame = false;
	}
#pragma endregion
}
// ========================================================
// 인터렉션 관련
// ========================================================
void APlayerCharacter::OnInteractionTriggerOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || !OtherActor->Implements<UInteractionInterface>()) return;

	InteractableActors.AddUnique(OtherActor);
	bEnableRayTrace = true;
}
void APlayerCharacter::OnInteractionTriggerOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor || !OtherActor->Implements<UInteractionInterface>()) return;

	InteractableActors.Remove(OtherActor);
	bEnableRayTrace = InteractableActors.Num() > 0;

	if (!bEnableRayTrace) ClearCurrentInteraction();
}
void APlayerCharacter::ClearCurrentInteraction()
{
	if (!InteractionActor) return;

	InteractionActor = nullptr;
	UpdateInteractionPrompt();
}
void APlayerCharacter::TraceForInteraction()
{
	FHitResult Hit;
	FCollisionQueryParams Params(TEXT("InteractionTrace"), true, this);

	const FVector Start = PlayerCamera->GetComponentLocation();
	const float Length = (PlayerCamera->GetComponentLocation() - SpringArm->GetComponentLocation()).Length() + InteractionTraceLength;
	const FVector End = Start + PlayerCamera->GetForwardVector() * Length;
	GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);
	//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 0.f, 0, 2.f);

	AActor* NewActor = nullptr;
	if (Hit.bBlockingHit) {
		AActor* HitActor = Hit.GetActor();

		if (HitActor && HitActor->Implements<UInteractionInterface>() &&
			IInteractionInterface::Execute_CanInteract(HitActor, this)) {
			NewActor = HitActor;
		}
	}

	if (InteractionActor != NewActor) {
		InteractionActor = NewActor;
		bHasValidInteraction = InteractionActor ? true : false;
		UpdateInteractionPrompt();
	}
}
void APlayerCharacter::UpdateInteractionPrompt()
{
	if (!InteractionActor) {
		OnInteractionPromptChanged.Broadcast(FText::GetEmpty());
		return;
	}

	const FText Text = IInteractionInterface::Execute_GetInteractionText(InteractionActor, this);
	OnInteractionPromptChanged.Broadcast(Text);
}
void APlayerCharacter::InputInteract()
{
	if (!InteractionActor) return;
	if (!InteractionActor->Implements<UInteractionInterface>()) return;

	if(bHasValidInteraction) IInteractionInterface::Execute_Interact(InteractionActor, this);
}
// ========================================================
// Input
// ========================================================
void APlayerCharacter::Move(const FInputActionValue& Value)
{
	// 공격 중이면 이동 불가
	if (bIsAttacking) return;

	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr) {
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}

	// [추가] 12/29, 걷거나 달리는 소리 재생
	FName RowName = bIsSprint ? FName("Sprint") : FName("Walk");
	USoundBase* TargetSound = GetSoundFromDataTable(RowName);

	if (TargetSound && MovementAudioComp)
	{
		// 이미 같은 소리가 나고 있으면 무시 (중복 재생 방지)
		if (MovementAudioComp->IsPlaying() && MovementAudioComp->Sound == TargetSound)
		{
			return;
		}

		// 소리 교체 및 재생
		MovementAudioComp->SetSound(TargetSound);
		MovementAudioComp->Play();
	}
}
void APlayerCharacter::StopMoveSound(const FInputActionValue& Value)
{
	// [추가] 12/29, 키보드 뗐으니 소리 끄기
	if (MovementAudioComp && MovementAudioComp->IsPlaying())
	{
		// 0.1초 동안 부드럽게 끄기 (뚝 끊김 방지)
		MovementAudioComp->FadeOut(0.1f, 0.0f);
	}
}
void APlayerCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr) 
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}
void APlayerCharacter::Roll(const FInputActionValue& Value)
{
	// 1. 필수 객체 체크 (없으면 아무것도 안 함)
	if (!AnimInstance.IsValid() || !GetController()) return;
	// 2. 이동 불가능 상태(스턴 등)면 구르기 불가
	if (GetController()->IsMoveInputIgnored()) return;
	// ========================================================
	// [핵심 수정] 연타 방지: 이미 구르기 몽타주가 재생 중이면 함수 종료!
	// ========================================================
	if (AnimInstance->Montage_IsPlaying(RollMontage)) return;
	// 3. 스태미너 체크
	if (StatComponent->GetStatCurrent(EFullStats::Stamina) < ConsumeStamina) return;
	// ========================================================
	// [공격 캔슬] 공격 중이었다면 강제로 멈추고 구르기 시작
	// ========================================================
	if (bIsAttacking)
	{
		// 변수 초기화
		bIsAttacking = false;
		bIsNextAttackRequested = false;
		CurrentCombo = 0;

		// 재생 중인 공격 몽타주 정지 (그래야 구르기가 즉시 섞여 들어감)
		if (CurrentComboMontage)
		{
			AnimInstance->Montage_Stop(0.2f, CurrentComboMontage);
		}
	}
	// 4. 구르기 실행 로직
	StatComponent->ChangeStatCurrent(EFullStats::Stamina, -ConsumeStamina);

	// [추가] 12/29, 구르는 동안 스태미나 회복을 멈춘다.
	StatComponent->SetStaminaRegenPaused(true);

	// 입력 방향으로 회전 (이동 키를 누르고 있을 때만)
	if (!GetLastMovementInputVector().IsNearlyZero())
	{
		// 입력한 방향으로 캐릭터를 즉시 회전시킵니다.
		FRotator InputRot = GetLastMovementInputVector().Rotation();
		SetActorRotation(InputRot);

		// 락온 중이라도 "타겟 바라보기"를 잠시 끕니다.
		// (안 끄면 Tick에서 다시 타겟 방향으로 강제 회전됨)
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}
	// 구르기 몽타주 재생
	PlayAnimMontage(RollMontage);

	// [추가] 12/29, 구르기 사운드 추가
	USoundBase* RollSound = GetSoundFromDataTable(FName("Roll"));
	if (RollSound)
	{
		UGameplayStatics::PlaySound2D(this, RollSound);
	}

	// 몽타주 종료 델리게이트 연결 (끝나면 원래대로 복구하기 위함)
	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &ThisClass::OnRollMontageEnded);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, RollMontage);
}
void APlayerCharacter::SetSprintMode(const FInputActionValue& Value)
{
	// 공격 중이면 달리기 시작 불가
	if (bIsAttacking) return;

	// 스태미너가 없으면 달리기 시작 불가
	if (StatComponent->GetStatCurrent(EFullStats::Stamina) <= 0.f) return;

	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	bIsSprint = true;
	StatComponent->StatsRegenMap[EFullStats::Stamina].TickRate = 0.f;

}
void APlayerCharacter::SetWalkMode(const FInputActionValue& Value)
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	bIsSprint = false;
	StatComponent->StatsRegenMap[EFullStats::Stamina].TickRate = StatComponent->STRegen;
}
void APlayerCharacter::EquipWeapon(const FInputActionValue& Value)
{
	// [추가] 공격 중이면 무기 교체 불가
	if (bIsAttacking) return;

	// 1. 입력값 가져오기 (IMC 스칼라 값: 1.0, 2.0, 3.0)
	float InputValue = Value.Get<float>();

	// 2. 정수로 변환 (1, 2, 3)
	int32 WeaponIndex = (int32)InputValue;

	/** 12/27 CurrentWeaponType이 존재하는 이유는?? */
	// 3. 숫자 -> Enum 변환
	//CurrentWeaponType = EWeaponCode::OneHandedSword;

	// 4. 내 컨트롤러 가져오기 (APRPlayerController로 형변환)
	APRPlayerController* PC = Cast<APRPlayerController>(GetController());

	// 5. 무기 교체 사운드 추가
	USoundBase* EquipWeaponSound = GetSoundFromDataTable(FName("EquipWeapon"));

	//[수정] 12/29, UI 업데이트 로직 통합
	if (PC)
	{
		// WeaponIndex는 1부터 시작하므로 -1을 해주면 0, 1, 2가 됨
		PC->UpdateHUDWeaponSlot(WeaponIndex - 1);
	}
	switch (WeaponIndex)
	{
	case 1:
		CurrentWeaponType = EWeaponCode::OneHandedSword;
		break;
	case 2:
		CurrentWeaponType = EWeaponCode::TwoHandedSword;
		break;
	case 3:
		CurrentWeaponType = EWeaponCode::DualBlade;
		break;
	default:
		return;
	}

	// 맵에서 해당 무기의 콤보 데이터 가져오기
	if (WeaponComboMap.Contains(CurrentWeaponType))
	{
		FWeaponComboData Data = WeaponComboMap[CurrentWeaponType];

		// 1. 몽타주 교체
		CurrentComboMontage = Data.Montage;
		// 2. 최대 콤보 수 교체 (3, 2, 4 등 설정값으로)
		MaxCombo = Data.MaxComboCount;

		// 3. 콤보 상태 초기화 (혹시 공격 중에 바꿀까봐)
		CurrentCombo = 0;
		bIsAttacking = false;
		bIsNextAttackRequested = false;
	}

	// =============================================================
	// 6. 실제 무기 교체 로직 (WeaponManager 활용)
	// =============================================================
	if (WeaponManager)
	{
		// A. 매니저에게서 해당 코드의 무기(미리 소환된 것)를 가져옵니다.
		AWeaponActor* NewWeapon = WeaponManager->GetWeaponInstance(CurrentWeaponType);

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

			// 무기 교체 사운드를 실행
			if (bCanPlayEquipSound && EquipWeaponSound)
			{
				UGameplayStatics::PlaySound2D(this, EquipWeaponSound);
			}

			// F. 현재 무기 변수 갱신 ("이제 이게 내 무기다!")
			CurrentWeapon = NewWeapon;
		}
	}
}
void APlayerCharacter::AttackInput(const FInputActionValue& Value)
{
	// 몽타주가 없으면 공격 불가
	if (!CurrentComboMontage)  {
		UE_LOG(LogTemp, Log, TEXT("몽타주가 없음"));
		return;
	}

	if (bIsAttacking) {
		// 공격 중이라면 다음 공격 예약 (최대 콤보 안 넘었을 때만)
		if (CurrentCombo < MaxCombo) bIsNextAttackRequested = true;
	}
	else {
		// 공격 시작
		PlayComboAttack();
	}
}
void APlayerCharacter::InputLockOn(const FInputActionValue& Value)
{
	if (LockOnComponent) LockOnComponent->ToggleLockOn();
}
void APlayerCharacter::ViewStat()
{
	if (APRPlayerController* PC = GetController<APRPlayerController>()) {
		PC->HandleStatInput();
	}
}
// ========================================================
// Combat & Combo
// ========================================================
void APlayerCharacter::PlayComboAttack()
{
	// 0. 몽타주가 없거나 애님 인스턴스가 없으면 취소
	if (!CurrentComboMontage) return;

	UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
	if (!AnimInst) return;

	// =============================================================
	// [버그 수정] 공격을 시작하면 강제로 달리기 상태 해제!
	// =============================================================
	if (bIsSprint)
	{
		bIsSprint = false;
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}
	// 공격 속도 멤버 변수 생성
	float AttackSpeed = 0.f;
	if (StatComponent)
	{
		// 현재 공격 속도를 StatComponent에서 가져와 저장
		AttackSpeed = StatComponent->GetStatCurrent(EFullStats::AttackSpeed);
		// 너무 느리거나 빨라지지 않게 안전장치 (0.5배 ~ 2.0배)
		AttackSpeed = FMath::Clamp(AttackSpeed, 0.5f, 2.0f);
	}

	// 스태미너 검사
	if (StatComponent && StatComponent->GetStatCurrent(EFullStats::Stamina) < ConsumeStamina)
	{
		return; // 스태미너 부족 시 공격 불가
	}

	//공격을 시도하면 무조건 스태미너 회복을 멈춥니다.
	StatComponent->SetStaminaRegenPaused(true);

	// =============================================================
	// 상황 1: 아무 몽타주도 재생 중이 아닐 때 (1타 공격 시작)
	// =============================================================
	if (!AnimInst->IsAnyMontagePlaying())
	{
		bIsAttacking = true;
		CurrentCombo = 1; // 1타부터 시작

		// 몽타주 재생 시 Speed 적용
		AnimInst->Montage_Play(CurrentComboMontage, AttackSpeed);

		// "Attack1" 섹션 재생
		FName SectionName = *FString::Printf(TEXT("Attack%d"), CurrentCombo);
		AnimInst->Montage_JumpToSection(SectionName, CurrentComboMontage);

		// [중요] 종료 델리게이트는 '처음 시작할 때' 한 번만 걸어주면 됩니다.
		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &ThisClass::OnAttackMontageEnded);
		AnimInst->Montage_SetEndDelegate(EndDelegate, CurrentComboMontage);

		StatComponent->ChangeStatCurrent(EFullStats::Stamina, -ConsumeStamina);
	}

	else if (AnimInst->GetCurrentActiveMontage() == CurrentComboMontage)
	{
		// 콤보 횟수 증가 (예: 1 -> 2)
		// MaxCombo를 넘지 않게 안전장치 (Clamp)
		CurrentCombo = FMath::Clamp(CurrentCombo + 1, 1, MaxCombo);

		// 다음 섹션 이름 만들기 (Attack2, Attack3...)
		FName SectionName = *FString::Printf(TEXT("Attack%d"), CurrentCombo);

		// 해당 섹션으로 점프 (이게 SectionJumpForCombo 역할)
		AnimInst->Montage_JumpToSection(SectionName, CurrentComboMontage);

		// 콤보 연계 시에도 속도가 바뀌었을 수 있으니 PlayRate 재설정
		AnimInst->Montage_SetPlayRate(CurrentComboMontage, AttackSpeed);

		StatComponent->ChangeStatCurrent(EFullStats::Stamina, -ConsumeStamina);
	}
}

void APlayerCharacter::ProcessWeaponHit(AActor* TargetActor)
{
	// 유효성 검사
	if (!TargetActor) return;
	// 2. 공격력 가져오기 (StatComponent 활용)
	float FinalDamage = 0.f;
	if (StatComponent)
	{
		FinalDamage = StatComponent->GetStatCurrent(EFullStats::PhysicalAttack);
	}
	// 3. 크리티컬 및 숙련도 계산
	if (WeaponMastery)
	{
		FWeaponMasteryData MasteryData = WeaponMastery->GetMasteryData(CurrentWeaponType);

		// 크리티컬 확률 체크
		if (FMath::FRand() <= MasteryData.CritRate)
		{
			FinalDamage *= MasteryData.CritDamage;

			if (CriticalCameraShakeClass)
			{
				if (APlayerController* PC = Cast<APlayerController>(GetController()))
				{
					PC->ClientStartCameraShake(CriticalCameraShakeClass);
				}
			}
			UE_LOG(LogTemp, Warning, TEXT("Critical Hit!"));
		}
	}
	// 4. 최종 데미지 전달 (부모 클래스의 Attack 함수 이용)
	Super::Attack(TargetActor, FinalDamage);
}

void APlayerCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// 몽타주가 끝났으니 상태 초기화
	bIsAttacking = false;
	bIsNextAttackRequested = false;
	CurrentCombo = 0;

	if (StatComponent) StatComponent->SetStaminaRegenPaused(false);
}

void APlayerCharacter::CheckComboInput()
{
	if (bIsNextAttackRequested)
	{
		bIsNextAttackRequested = false;
		PlayComboAttack(); // 다음 타격 실행
	}
}

void APlayerCharacter::OnAttackEnable(bool bEnable)
{
	if (CurrentWeapon)
	{
		CurrentWeapon->AttackEnable(bEnable, CurrentCombo);
	}
}

USoundBase* APlayerCharacter::GetSoundFromDataTable(FName RowName) const
{
	if (!SoundDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("SoundDataTable is not set!"));
		return nullptr;
	}

	// 테이블에서 RowName(예: "Potion", "Die")으로 데이터 찾기
	static const FString ContextString(TEXT("Sound Lookup"));
	FSoundData* Row = SoundDataTable->FindRow<FSoundData>(RowName, ContextString);

	if (Row) return Row->SoundAsset;

	return nullptr;
}

void APlayerCharacter::OnRollMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// 락온 컴포넌트가 있고, 현재 락온 중이라면 -> 다시 "타겟 바라보기" 모드로 복구
	if (LockOnComponent && LockOnComponent->bIsLockedOn)
	{
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}

	// [추가] 12/29, 구르기가 끝나면 스태미나 회복을 다시 실행한다.
	StatComponent->SetStaminaRegenPaused(false);
}

void APlayerCharacter::AddExp(float Amount)
{
	if(WeaponMastery) WeaponMastery->AddKillCount(CurrentWeaponType);
	
	Super::AddExp(Amount);
}
void APlayerCharacter::ReceiveDamage(float DamageAmount)
{
	//if (!StatComponent) return;
	if (bIsInvincible) return;
	Super::ReceiveDamage(DamageAmount);

	if (StatComponent->GetStatCurrent(EFullStats::Health) <= 0.f) Die();
}
void APlayerCharacter::Die()
{
	if (IsDeath == false && AnimInstance.IsValid()) {
		IsDeath = !IsDeath;
		PlayAnimMontage(DeathMontage);
		GetCharacterMovement()->DisableMovement();
		GetController()->SetIgnoreMoveInput(true);

		/** 12/27 해당 코드의 경우 재시작/메인메뉴버튼을 눌러야하는데, 해당코드들이 존재하지 않으면 클릭 불가능. */
		APlayerController* PC = Cast<APlayerController>(GetController());
		if (PC) {
			FInputModeUIOnly InputMode;
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock); // 필요하면 조절
			PC->SetInputMode(InputMode);
			PC->bShowMouseCursor = true;
		}

		USoundBase* DieSound = GetSoundFromDataTable(FName("Die"));
		if (DieSound)
		{
			UGameplayStatics::PlaySound2D(this, DieSound);
		}
	}

}
void APlayerCharacter::ShowDeathUI()
{
	if (YOUDIEWidget) return;

	if (YOUDIEWidgetClass) {
		YOUDIEWidget = CreateWidget<UPlayerDeathWidget>(GetWorld(), YOUDIEWidgetClass);
		if (YOUDIEWidget) 
			YOUDIEWidget->AddToViewport(10);
			YOUDIEWidget->PlayDeathAnimation();
	}
	
	// 유다이 위젯이 나오면서 사운드가 들림
	USoundBase* YouDieSound = GetSoundFromDataTable(FName("YouDie"));
	if (YouDieSound)
	{
		UGameplayStatics::PlaySound2D(this, YouDieSound);
	}

	// 시간 느려짐
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.5f);

	/** 12/27 전주 회의에서 유다이 때, retry/mainmenu로 가는것으로 확정되어 해당 코드 주석처리 */
	//GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &APlayerCharacter::RespawnPlayer, 2.0f, false);
}
void APlayerCharacter::AddPotion()
{
	// [추가] 12월 24일, 현재 포션 개수가 최대 포션 개수보다 많으면 그냥 리턴
	if (HPPotion >= MaxHPPotion) return;

	HPPotion++;
	OnPotionChanged.Broadcast(HPPotion);
}
void APlayerCharacter::EatPotion()
{
	if (0 < HPPotion) {
		// 필요 시 포션 먹는 몽타쥬 재생
		HPPotion--;
		StatComponent->ChangeStatCurrent(EFullStats::Health, RestoreHP);
		OnPotionChanged.Broadcast(HPPotion);

		USoundBase* PotionSound = GetSoundFromDataTable(FName("Potion"));
		if (PotionSound) UGameplayStatics::PlaySoundAtLocation(this, PotionSound, GetActorLocation());

		if (PotionEffectComponent) {
			// 이펙트를 껐다 켜서(Reset) 처음부터 재생되게 함
			PotionEffectComponent->Activate(true);
		}
	}
}
void APlayerCharacter::AddPotions(int32 Potion)
{
	HPPotion += Potion;
	OnPotionChanged.Broadcast(HPPotion);
}
void APlayerCharacter::RespawnPlayer()
{
	IsDeath = false;
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.f);

	SetActorLocation(SpawnLocation);
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC) {
		PC->SetIgnoreMoveInput(false);
		PC->SetIgnoreLookInput(false);

		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = false;
		PC->SetControlRotation(FRotator::ZeroRotator);
	}

	if (YOUDIEWidget) {
		YOUDIEWidget->RemoveFromParent();
		YOUDIEWidget = nullptr;
	}

	// 콜리전 및 물리 복구
	UCapsuleComponent* Capsule = GetCapsuleComponent();
	Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Capsule->SetCollisionResponseToAllChannels(ECR_Block);
	Capsule->SetCollisionProfileName(TEXT("Pawn"));

	USkeletalMeshComponent* Skel = GetMesh();
	Skel->SetSimulatePhysics(false);
	Skel->SetCollisionProfileName(TEXT("CharacterMesh"));

	Skel->AttachToComponent(Capsule, FAttachmentTransformRules::SnapToTargetIncludingScale);
	Skel->SetRelativeLocation(FVector(0, 0, -90));
	Skel->SetRelativeRotation(FRotator(0, -90, 0));
	Skel->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->SetComponentTickEnabled(true);

	UPRGameInstance* GI = Cast<UPRGameInstance>(GetGameInstance());
	if (GI && GI->CurrentChallengeDoor) {
		GI->CurrentChallengeDoor->ResetDoorAndBoss();
	}

	StatComponent->ChangeStatCurrent(EFullStats::Health, StatComponent->GetStatMax(EFullStats::Health));
	StatComponent->ChangeStatCurrent(EFullStats::Stamina, StatComponent->GetStatMax(EFullStats::Stamina));
}

void APlayerCharacter::HandleStageEntered(EStageType NewStage)
{
	if (CurrentStage == NewStage) return;

	CurrentStage = NewStage;

	if (CurrentStage == EStageType::Stage2) SnowNiagara->Activate();
	else SnowNiagara->Deactivate();
}

bool APlayerCharacter::SavePlayerComponents()
{
	if (!WeaponManager || !WeaponMastery || !GetStatComponent()) return false;

	UPRGameInstance* GI = Cast<UPRGameInstance>(GetGameInstance());
	if (!GI) return false;

	GI->SavedWeaponMasteryData = WeaponMastery->SaveData();
	GI->SavedStatData = GetStatComponent()->SaveData();
	GI->bHasSavedStatData = true;

	return true;
}

bool APlayerCharacter::LoadPlayerComponents()
{
	UPRGameInstance* GI = Cast<UPRGameInstance>(GetGameInstance());
	if (!GI || !GI->bHasSavedStatData) return false;
	if (!StatComponent || !WeaponMastery) return false;

	StatComponent->LoadData(GI->SavedStatData);
	WeaponMastery->LoadData(GI->SavedWeaponMasteryData);

	return true;
}
