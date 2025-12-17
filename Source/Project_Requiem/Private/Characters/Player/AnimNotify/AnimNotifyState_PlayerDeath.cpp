#include "Characters/Player/AnimNotify/AnimNotifyState_PlayerDeath.h"
#include "Characters/Player/Character/PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void UAnimNotifyState_Death::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    if (!MeshComp) return;

    APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComp->GetOwner());
    if (!Player) return;

    UCapsuleComponent* Capsule = Player->GetCapsuleComponent();
    Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 충돌 비활성화
    Capsule->SetCollisionResponseToAllChannels(ECR_Ignore);      // 모든 채널 무시

    // 2. 캐릭터 무브먼트 중지
    Player->GetCharacterMovement()->DisableMovement();
    Player->GetCharacterMovement()->StopMovementImmediately(); // 관성 제거

    // 3. 메쉬 설정
    USkeletalMeshComponent* Skel = Player->GetMesh();

    // 중요: 루트 컴포넌트(Capsule)로부터 메쉬를 분리하거나 
    // 메쉬가 더 이상 부모의 위치를 강제로 따라가지 않게 설정할 수 있습니다.
    // Skel->SetCollisionObjectType(ECC_PhysicsBody); 
    Player->GetCharacterMovement()->StopMovementImmediately();
    Skel->SetPhysicsLinearVelocity(FVector::ZeroVector);
    Skel->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

    Skel->SetCollisionProfileName(TEXT("Ragdoll"));
    Skel->SetSimulatePhysics(true);
    Skel->SetAllBodiesSimulatePhysics(true);
    Skel->WakeAllRigidBodies();

    // UI 표시 함수 호출
    Player->ShowDeathUI();
}