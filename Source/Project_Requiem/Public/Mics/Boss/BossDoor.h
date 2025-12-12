#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossDoor.generated.h"

// 델리게이트: 문 열림 후 보스에게 알림
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBossDoorOpened);

UCLASS()
class PROJECT_REQUIEM_API ABossDoor : public AActor
{
	GENERATED_BODY()
	
#pragma region 델리게이트
public:
	// 문 열림 이벤트 델리게이트
	UPROPERTY(BlueprintAssignable)
	FOnBossDoorOpened OnDoorOpened;
#pragma endregion

#pragma region 언리얼 생성
public:
	ABossDoor();
protected:
	virtual void BeginPlay() override;
#pragma endregion


    /// Todo : 보스에서 해야 하는것들
    /*
    //.h

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBossInit, ABaseCharacter*, Boss);

    UPROPERTY(BlueprintAssignable)
    FOnBossInit OnBossInit;

    UFUNCTION()
    void OnDoorOpened(ABaseCharacter* Boss);
    UPROPERTY(EditAnywhere)
    ABossDoor* ConnectedDoor;

    // .cpp Door에서 호출될 함수

    #include "Mics/Boss/BossDoor.h"

    void ABossCharacter::BeginPlay()
    {
        Super::BeginPlay();

        if (ConnectedDoor)
        {
            ConnectedDoor->OnDoorOpened.AddDynamic(this,);
        }
    }

    void ABossCharacter::OnDoorOpened()
    {
        // Hidden 등 등장처리(컷신 이후)

        OnBossInit.Broadcast(this);


        if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
        {
            if (APRPlayerController* PRPC = Cast<APRPlayerController>(PC))
            {
                this->OnBossInit.AddDynamic(PRPC, &APRPlayerController::OnEnterBossRoom);
            }
        }
    }
    */


#pragma region Door 기능
public:
    // 플레이어가 E를 눌렀을 때 호출
    void Interact();
    // 컷신 재생
    void PlayCutscene();
    // 컷신 종료 처리
    void OnCutsceneFinished();

protected:
    // 문 메시
    UPROPERTY(VisibleAnywhere)
    USkeletalMeshComponent* DoorMesh;

    // 컷신 재생용 애니메이션
    UPROPERTY(EditAnywhere)
    UAnimationAsset* DoorOpenAnim;
#pragma endregion
};
