#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/Characters/InteractionInterface.h"
#include "Characters/Player/Character/PlayerCharacter.h"
#include "Interface/Boss/BossControlInterface.h"
#include "BossBase/BossBase.h" /** 12/27 추가 */
#include "BossDoor.generated.h"

UCLASS()
class PROJECT_REQUIEM_API ABossDoor : public AActor, public IInteractionInterface
{
    GENERATED_BODY()

#pragma region Unreal Create
public:
    ABossDoor();
protected:
    virtual void BeginPlay() override;
#pragma endregion

#pragma region Interaction Interface
public:
    /** 플레이어가 상호작용 가능한지 (이미 열렸으면 false) */
    virtual bool CanInteract_Implementation(const APlayerCharacter* Caller) const override;

    /** 상호작용 메시지 (보스 처치 여부에 따라 다르게 설정 가능) */
    virtual FText GetInteractionText_Implementation(const APlayerCharacter* Caller) const override;

    /** 인터랙션 실행 */
    virtual void Interact_Implementation(APlayerCharacter* Caller) override;
#pragma endregion

#pragma region Door Logic
public:
    /** 문 열기 시작 (애니메이션/컷신 트리거) */
    UFUNCTION(BlueprintCallable, Category = "BossDoor")
    void OpenBossDoor();

    /** 보스가 죽었을 때 호출될 함수*/
    UFUNCTION()
    void OnBossDefeated();
    /** 보스에게 플레이어가 죽었을때, 호출될 함수*/
    UFUNCTION()
    void ResetDoorAndBoss();

protected:
    /** 실제 문을 여는 연출 (블루프린트 이벤트에서 오버라이드) */
    UFUNCTION(BlueprintImplementableEvent, Category = "BossDoor")
    void PlayDoorOpenAnimation();
    /** 실제 문을 닫는 연출 (블루프린트 이벤트에서 오버라이드) */
    UFUNCTION(BlueprintImplementableEvent, Category = "BossDoor")
    void PlayDoorCloseAnimation();

    /** 즉시 문을 열린 상태로 설정 (이미 클리어한 경우) */
    UFUNCTION(BlueprintCallable, Category = "BossDoor")
    void SetDoorOpenInstant();

    /** 보스문 앞에 설치될 캡슐 컴포넌트(인터렉션 안사용할 시)*/
    UFUNCTION()
    void OnFrontTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
    /** 보스문 뒤에 설치될 캡슐 컴포넌트(문닫기 전용)*/
    UFUNCTION()
    void OnEndTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
    /** 문이 현재 열려있는 상태인지 */
    UPROPERTY(VisibleAnywhere, Category = "BossDoor|State")
    bool bIsOpened = false;

    /** 보스를 이미 처치했는지 (세이브 데이터 등에서 받아올 값) */
    UPROPERTY(VisibleAnywhere, Category = "BossDoor|State")
    bool bIsBossCleared = false;
#pragma endregion

#pragma region Components
protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USceneComponent* Root;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* LeftDoorMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* RightDoorMesh;

    /** 입장 트리거 (보스전 시작 트리거로 활용 가능) */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UCapsuleComponent* FrontTrigger;

    /** 퇴장 트리거 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UCapsuleComponent* EndTrigger;
#pragma endregion

#pragma region Boss Reference
protected:
    /** 감시할 보스 클래스 */
    UPROPERTY(EditAnywhere, Category = "BossDoor|Setting")
    TSubclassOf<ACharacter> BossClass;

    /** 레벨에 스폰된 보스 캐싱 */
    UPROPERTY()
    ACharacter* CachedBoss = nullptr;
#pragma endregion

#pragma region Settings
    /** 인터렉션 모드 사용 여부 (true면 E키로 열기, false면 트리거 진입 시 자동) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BossDoor|Setting")
    bool bUseInteraction = true;
#pragma endregion

#pragma region Lighting
protected:
    /** 제어할 포인트 라이트 컴포넌트들 */
    UPROPERTY(EditAnywhere, Category = "BossDoor|Lighting")
    TArray<class APointLight*> BossPointLights;

    /** 목표 조도 (얼마나 밝게 켤 것인가) */
    UPROPERTY(EditAnywhere, Category = "BossDoor|Light")
    float MaxIntensity = 5000.f;

    UPROPERTY(EditAnywhere, Category = "BossDoor|Light")
    float IntensityStep = 100.f;

    float CurrentIntensity = 0.f;
    int32 IntensityDirection = -1; // 처음엔 감소
    
    UPROPERTY(EditAnywhere, Category = "BossDoor|Light")
    float LightInterval = 0.1f;

    bool bLightOn = false;
    /** 조명을 순차적으로 조절하기 위한 타이머 */
    FTimerHandle LightTimerHandle;
    int32 CurrentLightIndex = 0;

    /** 조도를 변경하는 실제 함수 */
    void UpdateLightIntensity();

    /** 리셋 시 조도를 0으로 만드는 함수 */
    void ResetLightsIntensity();
#pragma endregion

};