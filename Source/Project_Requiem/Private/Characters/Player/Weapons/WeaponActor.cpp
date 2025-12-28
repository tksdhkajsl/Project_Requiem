#include "Characters/Player/Weapons/WeaponActor.h"
#include "Components/BoxComponent.h"
#include "Components/AudioComponent.h"
#include "Characters/Player/Character/PlayerCharacter.h"
#include "Characters/Player/Weapons/WeaponMasteryComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Stats/StatComponent.h"

AWeaponActor::AWeaponActor()
{
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

	WeaponSlashEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Effect"));
	WeaponSlashEffect->SetupAttachment(WeaponMesh);
	WeaponSlashEffect->bAutoActivate = false;

	// 2. 왼손 (Sub)
	LeftWeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LeftMesh"));
	LeftWeaponMesh->SetupAttachment(root);
	LeftWeaponMesh->SetCollisionProfileName(TEXT("NoCollision"));

	LeftWeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftCollision"));
	LeftWeaponCollision->SetupAttachment(LeftWeaponMesh);
	LeftWeaponCollision->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));

	WeaponSlashEffectLeft = CreateDefaultSubobject<UNiagaraComponent>(TEXT("EffectLeft"));
	WeaponSlashEffectLeft->SetupAttachment(LeftWeaponMesh);
	WeaponSlashEffectLeft->bAutoActivate = false;

	WeaponAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("WeaponAudio"));
	WeaponAudioComponent->SetupAttachment(WeaponMesh);
	WeaponAudioComponent->bAutoActivate = false;

}
void AWeaponActor::BeginPlay()
{
	Super::BeginPlay();

	// 게임 시작 시 콜리전 강제 비활성화 (맨 처음에 무기가 그냥 닿아도 데미지가 들어가는 버그가 있음)
	if (WeaponCollision) WeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (LeftWeaponCollision) LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AdjustMeshToSocket(WeaponMesh);
	AdjustMeshToSocket(LeftWeaponMesh);

	if (WeaponCollision) WeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnWeaponBeginOverlap);
	if (LeftWeaponCollision) LeftWeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnWeaponBeginOverlap);	
}

void AWeaponActor::OnWeaponBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 1. 유효성 검사 (자기 자신, 주인, 이미 때린 적 제외)
	if (!OtherActor || OtherActor == this || OtherActor == GetOwner()) return;

	// 2. 중복 타격 방지
	if (HitActors.Contains(OtherActor)) return;
	HitActors.Add(OtherActor);

	// 3. 주인(Player)에게 "적중했다"고 알림
	APlayerCharacter* Player = Cast<APlayerCharacter>(GetOwner());
	if (Player)
	{
		// 무기는 '누굴 때렸는지'만 전달하고 빠집니다.
		Player->ProcessWeaponHit(OtherActor);
	}
}

void AWeaponActor::AttackEnable(bool bEnable, int32 ComboCount)
{
	if (bEnable) {
		// 1. 공격 시작: 맞은 목록 초기화
		HitActors.Empty();

		// 2. 콜리전 켜기 (QueryOnly: 물리충돌X, 이벤트O)
		if (WeaponCollision) WeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		if (LeftWeaponCollision) LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		// 3. 나이아가라 켜기
		if (WeaponSlashEffect) WeaponSlashEffect->Activate(true);
		if (WeaponSlashEffectLeft) WeaponSlashEffectLeft->Activate(true);

		// 4. 공격 소리 재생
		if (WeaponAudioComponent && SwingSounds.Num() > 0) {
			// ComboCount는 1부터 시작하므로, 배열 인덱스(0부터 시작)로 변환합니다.
			// (ComboCount - 1)
			// 만약 소리 개수보다 콤보가 길면 다시 처음 소리로 돌아가게(순환) 하려면 % 연산자 사용
			int32 SoundIndex = (ComboCount - 1) % SwingSounds.Num();

			// 인덱스 안전장치 (혹시라도 음수가 나오거나 범위 벗어남 방지)
			if (SwingSounds.IsValidIndex(SoundIndex) && SwingSounds[SoundIndex])
			{
				WeaponAudioComponent->SetSound(SwingSounds[SoundIndex]);
				WeaponAudioComponent->Play();
			}
		}
	}
	else {
		// 1. 공격 끝: 콜리전 끄기
		if (WeaponCollision) WeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if (LeftWeaponCollision) LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// 2. 나이아가라 끄기
		if (WeaponSlashEffect) WeaponSlashEffect->Deactivate();
		if (WeaponSlashEffectLeft) WeaponSlashEffectLeft->Deactivate();

		// 3. 목록 비우기 (안전장치)
		HitActors.Empty();
	}
}

void AWeaponActor::UpdateVFXByRank(EWeaponRank NewRank)
{
	// 1. 맵에서 해당 랭크의 이펙트 에셋을 찾습니다.
// (맵에 키가 없으면 nullptr가 반환됩니다)
	UNiagaraSystem* NewEffect = RankVFXMap.FindRef(NewRank);

	// 2. 이펙트가 있으면 -> 켜기
	if (NewEffect) {
		// 오른손
		if (WeaponSlashEffect) WeaponSlashEffect->SetAsset(NewEffect);
		// 왼손 (쌍검)
		if (WeaponSlashEffectLeft) WeaponSlashEffectLeft->SetAsset(NewEffect);
	}
	// 3. 이펙트가 없으면(F랭크 or 빈칸) -> 끄기
	else {
		if (WeaponSlashEffect) WeaponSlashEffect->SetAsset(nullptr); // 에셋 비우기
		if (WeaponSlashEffectLeft) WeaponSlashEffectLeft->SetAsset(nullptr);
	}
}

void AWeaponActor::AdjustMeshToSocket(USkeletalMeshComponent* InMesh)
{
	// 1. 소켓 이름이 비어있거나, 메시가 없으면 패스
	if (!InMesh || GripSocketName.IsNone()) return;

	// 2. 해당 소켓이 있는지 확인
	if (InMesh->DoesSocketExist(GripSocketName)) {
		// 3. 소켓의 트랜스폼을 가져옴
		FTransform SocketTransform = InMesh->GetSocketTransform(GripSocketName, RTS_Component);

		// 4. 역행렬(반대 위치) 계산
		FTransform InverseTransform = SocketTransform.Inverse();

		// 5. 메시 위치를 조정해서 소켓이 (0,0,0)에 오게 만듦
		InMesh->SetRelativeTransform(InverseTransform);
	}
}



