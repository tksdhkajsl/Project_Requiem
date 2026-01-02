#include "Characters/Player/Weapons/WeaponMasteryComponent.h"
#include "Characters/Player/Character/PlayerCharacter.h" 
#include "Characters/Player/Weapons/WeaponActor.h"

UWeaponMasteryComponent::UWeaponMasteryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}
void UWeaponMasteryComponent::BeginPlay()
{
	Super::BeginPlay();

	// 시작 시 기본 스탯(F랭크)으로 초기화
	UpdateStatsByRank(OneHandedMastery);
	UpdateStatsByRank(TwoHandedMastery);
	UpdateStatsByRank(DualBladeMastery);
}
void UWeaponMasteryComponent::AddKillCount(EWeaponCode WeaponType)
{
	switch (WeaponType) {
	case EWeaponCode::OneHandedSword:
	{
		OneHandedMastery.CurrentKillCount++;
		CheckRankUp(OneHandedMastery, WeaponType);
	}
		break;
	case EWeaponCode::TwoHandedSword:
	{
		TwoHandedMastery.CurrentKillCount++;
		CheckRankUp(TwoHandedMastery, WeaponType);
	}
		break;
	case EWeaponCode::DualBlade: // 쌍검
	{
		DualBladeMastery.CurrentKillCount++;
		CheckRankUp(DualBladeMastery, WeaponType);
	}
		break;
	default:
		break;
	}
}
//데이터 가져오기 함수
FWeaponMasteryData UWeaponMasteryComponent::GetMasteryData(EWeaponCode WeaponType) const
{
	switch (WeaponType) {
	case EWeaponCode::OneHandedSword: return OneHandedMastery;
	case EWeaponCode::TwoHandedSword: return TwoHandedMastery;
	case EWeaponCode::DualBlade:      return DualBladeMastery;
	}
	return FWeaponMasteryData(); // 기본값 반환
}
EWeaponRank UWeaponMasteryComponent::GetWeaponRank(EWeaponCode WeaponType) const
{
	switch (WeaponType)
	{
	case EWeaponCode::OneHandedSword:
		return OneHandedMastery.CurrentRank;

	case EWeaponCode::TwoHandedSword:
		return TwoHandedMastery.CurrentRank;

	case EWeaponCode::DualBlade:
		return DualBladeMastery.CurrentRank;

	default:
		return EWeaponRank::F; // 예외 처리
	}
}
void UWeaponMasteryComponent::CheckRankUp(FWeaponMasteryData& MasteryData, EWeaponCode WeaponType)
{
	// S랭크면 더 이상 성장 안 함
	if (MasteryData.CurrentRank == EWeaponRank::S) return;

	// 현재 킬 수가 목표(Max)를 넘었는지 확인
	if (MasteryData.CurrentKillCount >= MasteryData.MaxKillCount)
	{
		// 랭크 승급 (Enum 값을 1 증가)
		uint8 NextRankByte = (uint8)MasteryData.CurrentRank + 1;
		MasteryData.CurrentRank = (EWeaponRank)NextRankByte;

		// 킬 카운트 초기화 (누적식이면 이 줄 삭제)
		MasteryData.CurrentKillCount = 0;

		// 바뀐 랭크에 맞춰 스탯(크확, 크뎀, 다음 목표) 갱신
		UpdateStatsByRank(MasteryData);

		UE_LOG(LogTemp, Log, TEXT("Weapon Rank Up! New Rank: %d"), (int32)MasteryData.CurrentRank);

		APlayerCharacter* Player = Cast<APlayerCharacter>(GetOwner());

		if (Player)
		{
			// =================================================================
			// [A] VFX 업데이트 (들고 있는 무기일 경우)
			// =================================================================
			if (Player->GetCurrentWeaponType() == WeaponType)
			{
				if (AWeaponActor* CurrentWeapon = Player->GetCurrentWeapon())
				{
					CurrentWeapon->UpdateVFXByRank(MasteryData.CurrentRank);
				}
			}
		}
	}
}

void UWeaponMasteryComponent::UpdateStatsByRank(FWeaponMasteryData& MasteryData)
{
	switch (MasteryData.CurrentRank)
	{
	case EWeaponRank::F: //[수정] 12/29, 크리티컬 확률: 0%, 크리티컬 데미지 : 1.0배, 목표 3마리
	{
		MasteryData.CritRate = 0.0f;
		MasteryData.CritDamage = 1.0f;
		MasteryData.MaxKillCount = 3;
	}
		break;
	case EWeaponRank::E: //[수정] 12/29, 크리티컬 확률: 10%, 크리티컬 데미지 : 1.2배, 목표 5마리
	{
		MasteryData.CritRate = 0.1f;
		MasteryData.CritDamage = 1.2f;
		MasteryData.MaxKillCount = 5;
	}
		break;
	case EWeaponRank::D: //[수정] 12/29, 크리티컬 확률: 20%, 크리티컬 데미지 : 1.4배, 목표 7마리
	{
		MasteryData.CritRate = 0.2f;
		MasteryData.CritDamage = 1.4f;
		MasteryData.MaxKillCount = 7;
	}
		break;
	case EWeaponRank::C: //[수정] 12/29, 크리티컬 확률: 30%, 크리티컬 데미지 : 1.5배, 목표 9마리
	{
		MasteryData.CritRate = 0.3f;
		MasteryData.CritDamage = 1.5f;
		MasteryData.MaxKillCount = 9;
	}
		break;
	case EWeaponRank::B: //[수정] 12/29, 크리티컬 확률: 40%, 크리티컬 데미지 : 1.6배, 목표 11마리
	{
		MasteryData.CritRate = 0.4f;
		MasteryData.CritDamage = 1.6f;
		MasteryData.MaxKillCount = 11;
	}
		break;
	case EWeaponRank::A: //[수정] 12/29, 크리티컬 확률: 45%, 크리티컬 데미지 : 1.8배, 목표 13마리
	{
		MasteryData.CritRate = 0.45f;
		MasteryData.CritDamage = 1.8f;
		MasteryData.MaxKillCount = 13;
	}
		break;
	case EWeaponRank::S: //[수정] 12/29, 크리티컬 확률: 50%, 크리티컬 데미지 : 2.0배, 끝
	{
		MasteryData.CritRate = 0.5f;
		MasteryData.CritDamage = 2.0f;
		MasteryData.MaxKillCount = 999999; // 맥스
	}
		break;
	}
}



