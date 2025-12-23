#include "Mics/Boss/BossDoor.h"
#include "Components/SkeletalMeshComponent.h"
#include "TimerManager.h"

// ========================================================
// 언리얼 생성
// ========================================================
ABossDoor::ABossDoor()
{
    PrimaryActorTick.bCanEverTick = false;
    DoorMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("DoorMesh"));
    RootComponent = DoorMesh;
}
void ABossDoor::BeginPlay()
{
    Super::BeginPlay();
}
// ========================================================
// HUD 위젯 클래스
// ========================================================
void ABossDoor::Interact()
{
    PlayCutscene();
}
void ABossDoor::PlayCutscene()
{
    if (!DoorOpenAnim) return;

    DoorMesh->PlayAnimation(DoorOpenAnim, false);

    // 컷신 길이만큼 타이머 설정
    float AnimLength = DoorOpenAnim->GetMaxCurrentTime();
    GetWorldTimerManager().SetTimerForNextTick([this, AnimLength]()
        {
            FTimerHandle TimerHandle;
            GetWorldTimerManager().SetTimer(
                TimerHandle,
                this,
                &ABossDoor::OnCutsceneFinished,
                AnimLength,
                false
            );
        });
}
void ABossDoor::OnCutsceneFinished()
{
    OnDoorOpened.Broadcast();
}
// ========================================================
// HUD 위젯 클래스
// ========================================================
bool ABossDoor::CanInteract_Implementation(const APlayerCharacter* Caller) const
{
    return !bIsOpened;
}
FText ABossDoor::GetInteractionText_Implementation(const APlayerCharacter* Caller) const
{
    return FText::FromString(TEXT("보스방 문 열기"));
}
void ABossDoor::Interact_Implementation(APlayerCharacter* Caller)
{
    if (bIsOpened) return;

    bIsOpened = true;
    PlayCutscene();
}