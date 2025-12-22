#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/Characters/InteractionInterface.h"
#include "Characters/Player/Character/PlayerCharacter.h"
#include "BossDoor.generated.h"


/*
 class PROJECT_REQUIEM_API 본인들이 만든 클래스 
 : public AActor, /*!!인터페이스 상속 public IBossControlInterface */
/*
// 1. 시작 시 위치 기억
void ABossBase::BeginPlay() 
{
    Super::BeginPlay();
    
    // 필요시 로테이션도 저장!
    InitialLocation = GetActorLocation();
}

// 2. 문 열릴 때 호출 (깨우기)
void ABossBase::ActivateBossBattle() 
{
    // 이하 코드는 수도(의사)코드니 본인들의 로직에 맞게 변경 요망.
    // AI 활성화 (팀원이 쓰는 AIController 변수명 확인)
    if (AAIController* AIC = Cast<AAIController>(GetController())) {
        AIC->GetBrainComponent()->RestartLogic(); // 비헤이비어 트리 다시 시작
    }
    // 공격성, 이동 속도 등 전투 상태로 변경
    GetCharacterMovement()->MaxWalkSpeed = 600.f;
    // UI 띄우기 등 추가 (예: Widget->SetVisibility(Visible))

    // 여기서 최초로 broadcast로 2가지
    // 1. 현재 체력, Max체력 => 체력 깍일때 마다, 무조건 broadcast
    // 2. 보스몹 이름(FText) => 굳이 계속 하면 메모리 아까우니 한번만
}

// 3. 플레이어 패배 시 호출 (다시 재우기)
void ABossBase::ResetBossToDefault() 
{
    // 이하 코드는 수도(의사)코드니 본인들의 로직에 맞게 변경 요망.
    // AI 끄기
    if (AAIController* AIC = Cast<AAIController>(GetController())) {
        AIC->GetBrainComponent()->StopLogic("Player Defeated");
    }

    // 위치 초기화 및 체력 회복
    SetActorLocation(InitialLocation);
    SetActorRotation(FRotator::ZeroRotator);

    // 보스 스탯 컴포넌트가 있다면 (예시) => 체력 풀(Full)로 채우기
    // StatComponent->SetHP(StatComponent->GetMaxHP());

    // 애니메이션 몽타주 중지하고 Idle로
    GetMesh()->GetAnimInstance()->StopAllMontages(0.2f);

    // 플레이어 HUD초기화는 플레이어가 담당.
}
*/


UCLASS()
class PROJECT_REQUIEM_API ABossDoor : public AActor, public IInteractionInterface
{
    GENERATED_BODY()

public:
    ABossDoor();

protected:
    virtual void BeginPlay() override;

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
};