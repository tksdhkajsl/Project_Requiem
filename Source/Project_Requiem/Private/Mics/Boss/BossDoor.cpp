#include "Mics/Boss/BossDoor.h"

#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EngineUtils.h"
#include "GameFramework/Character.h"
#include "Interface/Boss/BossControlInterface.h"
//#include "Core/PRGameInstance.h"

ABossDoor::ABossDoor()
{
    PrimaryActorTick.bCanEverTick = false;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

    LeftDoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftDoorMesh"));
    LeftDoorMesh->SetupAttachment(Root);

    RightDoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightDoorMesh"));
    RightDoorMesh->SetupAttachment(Root);

    FrontTrigger = CreateDefaultSubobject<UCapsuleComponent>(TEXT("FrontTrigger"));
    FrontTrigger->SetupAttachment(Root);
    FrontTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
    FrontTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    EndTrigger = CreateDefaultSubobject<UCapsuleComponent>(TEXT("EndTrigger"));
    EndTrigger->SetupAttachment(Root);
    EndTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
    EndTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void ABossDoor::BeginPlay()
{
    Super::BeginPlay();

    if (bIsBossCleared) SetDoorOpenInstant();
    else {
        if (BossClass) {
            for (TActorIterator<ACharacter> It(GetWorld(), BossClass); It; ++It) {
                CachedBoss = *It;
                break;
            }
        }
    }

    FrontTrigger->OnComponentBeginOverlap.AddDynamic(this, &ABossDoor::OnFrontTriggerBeginOverlap);
    EndTrigger->OnComponentBeginOverlap.AddDynamic(this, &ABossDoor::OnEndTriggerBeginOverlap);

    if (bIsBossCleared) SetDoorOpenInstant();
}

bool ABossDoor::CanInteract_Implementation(const APlayerCharacter* Caller) const
{
    // 이미 열려있거나 보스를 깼다면 인터랙션 불필요
    return !bIsOpened && !bIsBossCleared;
}
FText ABossDoor::GetInteractionText_Implementation(const APlayerCharacter* Caller) const
{
    return bUseInteraction ? FText::FromString(TEXT("보스방 문 열기")) : FText::GetEmpty();
}
void ABossDoor::Interact_Implementation(APlayerCharacter* Caller)
{
    if (!bUseInteraction) return;
    if (CanInteract_Implementation(Caller))
    {
        OpenBossDoor();
    }
}

void ABossDoor::OpenBossDoor()
{
    if (bIsOpened) return;
    bIsOpened = true;

    //if (UPRGameInstance* GI = Cast<UPRGameInstance>(GetGameInstance())) {
    //    GI->CurrentChallengeDoor = this;
    //}

    PlayDoorOpenAnimation();
}
void ABossDoor::OnBossDefeated()
{
    bIsBossCleared = true;

    if (!bIsOpened) OpenBossDoor();
}
void ABossDoor::ResetDoorAndBoss()
{
    bIsOpened = false;
    EndTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

    if (CachedBoss) {
        if (IBossControlInterface* BossInterface = Cast<IBossControlInterface>(CachedBoss)) {
            BossInterface->ResetBossToDefault();
        }
    }
}
void ABossDoor::SetDoorOpenInstant()
{
    bIsOpened = true;
}

void ABossDoor::OnFrontTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor || bIsOpened) return;
    if (!OtherActor->IsA<APlayerCharacter>()) return;
    if (bUseInteraction) return;

    OpenBossDoor();
}

void ABossDoor::OnEndTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor || bIsBossCleared) return;
    if (!OtherActor->IsA<APlayerCharacter>()) return;

    if (CachedBoss) {
        IBossControlInterface* BossInterface = Cast<IBossControlInterface>(CachedBoss);
        if (BossInterface) {
            BossInterface->ActivateBossBattle();

            PlayDoorCloseAnimation();

            EndTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        }
    }
}