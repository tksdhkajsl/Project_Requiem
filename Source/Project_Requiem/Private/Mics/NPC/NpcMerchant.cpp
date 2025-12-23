#include "Mics/NPC/NpcMerchant.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "TimerManager.h"

// ========================================================
// 언리얼 생성
// ========================================================
ANpcMerchant::ANpcMerchant()
{
    PrimaryActorTick.bCanEverTick = false;
    Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
    RootComponent = Mesh;

    InteractionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionBox"));
    InteractionBox->SetupAttachment(RootComponent);

    InteractionBox->SetBoxExtent(FVector(60.f, 60.f, 100.f));
    InteractionBox->SetRelativeLocation(FVector(0.f, 0.f, 80.f));

    InteractionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    InteractionBox->SetCollisionObjectType(ECC_WorldDynamic);
    InteractionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    InteractionBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    InteractionBox->SetGenerateOverlapEvents(false);
}
void ANpcMerchant::BeginPlay()
{
    Super::BeginPlay();
}
// ========================================================
// HUD 위젯 클래스
// ========================================================
bool ANpcMerchant::CanInteract_Implementation(const APlayerCharacter* Caller) const
{
    return true;
}
FText ANpcMerchant::GetInteractionText_Implementation(const APlayerCharacter* Caller) const
{
    return FText::FromString(TEXT("상인"));
}
void ANpcMerchant::Interact_Implementation(APlayerCharacter* Caller)
{
    if (!Caller) return;

    // 1. 입력 잠금
    // 2. 상점 UI 열기
    // 3. 카메라 고정 or 회전 제한
}