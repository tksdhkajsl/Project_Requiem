// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Weapon/WeaponCodeEnum.h"

#include "Animation/AnimInstance.h"

#include "PlayerCharacter.generated.h"

class UInputConfig;
class USceneCaptureComponent2D;
class UPlayerDeathWidget;
class UCameraShakeBase;
struct FInputActionValue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionPromptChanged, const FText&, Text);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPotionChanged, int32, NewPotionCount);


UCLASS()
class PROJECT_REQUIEM_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()
	
#pragma region 델리게이트
public:
	FOnInteractionPromptChanged OnInteractionPromptChanged;
	FOnPotionChanged OnPotionChanged;
#pragma endregion
	
#pragma region 언리얼 기본 생성 및 초기화
public:
	APlayerCharacter();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;
	
	/*
	 * 25/12/17 코드 추가 : 변경자 천수호
	 * 변경 내용 : 인터렉션 가능한 액터들 모음
	 */
	UPROPERTY(VisibleAnywhere, Category = "Interaction")	
	TArray<AActor*> InteractableActors;
	/*
	 * 25/12/17 코드 추가 : 변경자 천수호
	 * 변경 내용 : 플레이어 주변으로 인터렉션이 가능한(InteractionInterface 상속받은) 액터가 들어옴
	 */
	UFUNCTION()
	void OnInteractionTriggerOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	/*
	 * 25/12/17 코드 추가 : 변경자 천수호
	 * 변경 내용 : 플레이어 주변으로 인터렉션이 가능한(InteractionInterface 상속받은) 액터가 빠져나감
	 */
	UFUNCTION()
	void OnInteractionTriggerOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	/*
	 * 25/12/17 코드 추가 : 변경자 천수호
	 * 변경 내용 : 플레이어 주변으로 인터렉션이 가능한(InteractionInterface 상속받은) 액터가 들어옴
	 */
	void ClearCurrentInteraction();
protected:
	virtual void BeginPlay() override;
#pragma endregion

#pragma region 인터렉션 관련
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bEnableRayTrace = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float InteractionTraceLength = 200;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USphereComponent* InteractionTrigger;
private:
	UPROPERTY()
	AActor* InteractionActor = nullptr;
	bool bHasValidInteraction = false;
	// 인터렉션 용
	void TraceForInteraction();
	void UpdateInteractionPrompt();
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
	void AttackInput(const FInputActionValue& Value);
	void InputLockOn(const FInputActionValue& Value);

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

#pragma region Combat & Combo
protected:
	// [추가 3] 무기별 콤보 데이터 (에디터에서 설정: Key=한손검, Value={몽타주, 3타})
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TMap<EWeaponCode, FWeaponComboData> WeaponComboMap;

	// 현재 장착된 무기에 맞는 몽타주 (EquipWeapon에서 갱신됨)
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> CurrentComboMontage = nullptr;

	// 현재 장착된 무기의 최대 콤보 수 (EquipWeapon에서 갱신됨)
	int32 MaxCombo = 0;

	// 현재 몇 번째 콤보인지 (1 ~ MaxCombo)
	int32 CurrentCombo = 0;

	// 공격 중인지 확인
	bool bIsAttacking = false;

	// 다음 공격 예약 여부 (선입력)
	bool bIsNextAttackRequested = false;

	// 콤보 공격 실행 함수
	void PlayComboAttack();
#pragma endregion

public:
	// 무기에서 호출할 충돌 처리 함수
	UFUNCTION()
	void ProcessWeaponHit(AActor* TargetActor);

	// 몽타주 종료 시 호출될 델리게이트
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	// AnimNotify_CheckComboInput 에서 호출할 함수
	UFUNCTION(BlueprintCallable)
	void CheckComboInput();

	// 외부(애님 인스턴스)에서 내 무기 타입을 물어볼 때 대답해주는 함수
	UFUNCTION(BlueprintCallable, Category = "Combat")
	FORCEINLINE EWeaponCode GetCurrentWeaponType() const { return CurrentWeaponType; }

	// 현재 들고 있는 무기를 반환하는 함수 (Getter)
	FORCEINLINE class AWeaponActor* GetCurrentWeapon() const { return CurrentWeapon; }

	// 노티파이가 공격을 가능하게 만들라는 신호가 왔을 때 실행될 함수
	void OnAttackEnable(bool bEnable);


protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|Camera")
	TObjectPtr<class USpringArmComponent> SpringArm = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|Camera")
	TObjectPtr<class UCameraComponent> PlayerCamera = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|Weapon")
	TObjectPtr<class UWeaponManagerComponent> WeaponManager = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player|Weapon")
	TObjectPtr<class UWeaponMasteryComponent> WeaponMastery = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class ULockOnComponent> LockOnComponent;

	// 구르기 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation|Montage")
	TObjectPtr<UAnimMontage> RollMontage = nullptr;

	// 현재 손에 들고 있는 무기
	UPROPERTY(VisibleInstanceOnly, Category = "Combat")
	TObjectPtr<class AWeaponActor> CurrentWeapon = nullptr;

	// [추가] 현재 장착 중인 무기 타입을 기억하는 변수
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat")
	EWeaponCode CurrentWeaponType = EWeaponCode::OneHandedSword; // 기본값 설정

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

protected:
	// [추가] 크리티컬 발생 시 사용할 카메라 셰이크 클래스 (블루프린트에서 할당)
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<UCameraShakeBase> CriticalCameraShakeClass;

	// [추가] 현재 공격 속도를 계산해서 가져오는 헬퍼 함수
	float GetAttackSpeedRate() const;

public:
	/*
	 * 25/12/17 코드 추가 : 변경자 천수호
	 * 변경 내용 : 적 킬 시 플레이어 무기의 랭크 올리기 위해 추가.
			      !부모 클래스에서는 캐스팅을 추가로 하여야 하기 때문에.
	 */
	virtual void AddExp(float Amount) final;
	/*
	 * 25/12/17 코드 추가 : 변경자 천수호
	 * 추가 내용 : 적에게 대미지 입을시 로직 구성
	 */
	virtual void ReceiveDamage(float DamageAmount) final;
	/*
	 * 25/12/17 코드 추가 : 변경자 천수호
	 * 추가 내용 : 체력이 0이하가 됬을 시 호출
	 */
	virtual void Die() final;
	/*
	 * 25/12/17 코드 추가 : 변경자 천수호
	 * 추가 내용 : 죽은 후 리스폰 시 부를 함수
	 */
	UFUNCTION(BlueprintCallable)
	void RespawnPlayer();
	
	/*
	 * 25/12/17 코드 추가 : 변경자 천수호
	 * 추가 내용 : 화롯불에 도착해서 인터렉션 할 경우 화롯불에서 해당 위치를 저장할 함수
	 */
	UFUNCTION(BlueprintCallable)
	void SetSpawnLocation(const FVector& spawnLocation) { SpawnLocation = spawnLocation; }
	/*
	 * 25/12/17 코드 추가 : 변경자 천수호
	 * 추가 내용 : 그냥 세터를 넣어서 넣음
	 */
	FVector GetSpawnLocation() { return SpawnLocation; }
	/*
	 * 25/12/17 코드 추가 : 변경자 천수호
	 * 추가 내용 : 플레이어가 다시 리스폰 할 위치
	 */
	FVector SpawnLocation;
	/*
	 * 25/12/17 코드 추가 : 변경자 천수호
	 * 추가 내용 : 죽었을 경우 유다이 띄우는 함수
	 */
	void ShowDeathUI();
	/*
	 * 25/12/17 코드 추가 : 변경자 천수호
	 * 추가 내용 : 구르기 몽타쥬와 같은 이유로 넣음(BP에서 넣기)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation|Montage")
	TObjectPtr<UAnimMontage> DeathMontage = nullptr;

	/*
	 * 25/12/17 코드 추가 : 변경자 천수호
	 * 추가 내용 : BP에서 설정할 유다이용 WBP
	 */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UPlayerDeathWidget> YOUDIEWidgetClass;
	/*
	 * 25/12/17 코드 추가 : 변경자 천수호
	 * 추가 내용 : 현재 생성된 유다이용 위젯
	 */
	UPROPERTY()
	TObjectPtr<UPlayerDeathWidget> YOUDIEWidget;

	/*
	 * 25/12/17 코드 추가 : 변경자 천수호
	 * 추가 내용 : 죽을 경우 또 죽는 애니메이션이 재생되기 때문에 
	 *            현재 죽었는지를 체크하는 변수
	 *            true = 죽음, false = 생존
	 */
	bool IsDeath = false;

	/*
	 * 25/12/17 코드 추가 : 변경자 천수호
	 * 추가 내용 : 포션(키보드 4번)
	 */
	int32 HPPotion = 3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RestoreHP = 30.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ConsumeStamina = 10.f;
	void AddPotion();
	void EatPotion();
	void AddPotions(int32 Potion);
	int32 GetHPPotion() const { return HPPotion; }
};
