// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/WeaponActor.h"
#include "Components/BoxComponent.h"
#include "Components/AudioComponent.h"
#include "Player/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"

// Sets default values
AWeaponActor::AWeaponActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(root);

	// 1. 오른손 (Main)
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	WeaponMesh->SetupAttachment(root);
	WeaponMesh->SetCollisionProfileName(TEXT("NoCollision"));

	WeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	WeaponCollision->SetupAttachment(WeaponMesh);
	WeaponCollision->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));

	// 2. 왼손 (Sub)
	LeftWeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LeftMesh"));
	LeftWeaponMesh->SetupAttachment(root);
	LeftWeaponMesh->SetCollisionProfileName(TEXT("NoCollision"));

	LeftWeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftCollision"));
	LeftWeaponCollision->SetupAttachment(LeftWeaponMesh);
	LeftWeaponCollision->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));

	WeaponSlashEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Effect"));
	WeaponSlashEffect->SetupAttachment(WeaponMesh);

	WeaponAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("WeaponAudio"));
	WeaponAudioComponent->SetupAttachment(WeaponMesh);
	WeaponAudioComponent->bAutoActivate = false;

}

// Called when the game starts or when spawned
void AWeaponActor::BeginPlay()
{
	Super::BeginPlay();

	AdjustMeshToSocket(WeaponMesh);
	AdjustMeshToSocket(LeftWeaponMesh);
	
}

void AWeaponActor::OnWeaponBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
}

void AWeaponActor::AdjustMeshToSocket(USkeletalMeshComponent* InMesh)
{
	// 1. 소켓 이름이 비어있거나, 메시가 없으면 패스
	if (!InMesh || GripSocketName.IsNone()) return;

	// 2. 해당 소켓이 있는지 확인
	if (InMesh->DoesSocketExist(GripSocketName))
	{
		// 3. 소켓의 트랜스폼을 가져옴
		FTransform SocketTransform = InMesh->GetSocketTransform(GripSocketName, RTS_Component);

		// 4. 역행렬(반대 위치) 계산
		FTransform InverseTransform = SocketTransform.Inverse();

		// 5. 메시 위치를 조정해서 소켓이 (0,0,0)에 오게 만듦
		InMesh->SetRelativeTransform(InverseTransform);
	}
}



