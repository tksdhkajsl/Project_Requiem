#include "Stats/StatComponent.h"
#include "TimerManager.h"

// ========================================================
// 편의용
// ========================================================
auto BroadcastStats = [&](const TMap<EFullStats, FCoreStat>& Map, auto& Delegate)
	{
		TArray<FStatEntry> Entries;
		for (auto& Pair : Map)
		{
			FStatEntry Entry;
			Entry.StatType = Pair.Key;
			Entry.StatValue = Pair.Value;
			Entries.Add(Entry);
		}
		Delegate.Broadcast(Entries);
	};
// ========================================================
// 언리얼 생성 및 초기화 함수
// ========================================================
UStatComponent::UStatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}
void UStatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!FMath::IsNearlyZero(TimerTick)) {
		GetWorld()->GetTimerManager().SetTimer(StatRegenTimerHandle, this, &UStatComponent::HandleStatRegeneration, TimerTick, true);
	}
}
void UStatComponent::InitializeStatsComponent()
{
	StatsRegenMap.Empty();
	StatsRegenMap.Add(EFullStats::Health, FCoreStat(CurrHP, MaxHP, HPRegen));
	StatsRegenMap.Add(EFullStats::Stamina, FCoreStat(CurrST, MaxST, STRegen));

	StatsNonRegenMap.Empty();
	StatsNonRegenMap.Add(EFullStats::PhysicalAttack, FCoreStat(PhyAtt, PhyAtt));
	StatsNonRegenMap.Add(EFullStats::AttackSpeed, FCoreStat(AttSpeed, AttSpeed));
	StatsNonRegenMap.Add(EFullStats::MagicAttack, FCoreStat(MagAtt, MagAtt));
	StatsNonRegenMap.Add(EFullStats::PhysicalDefense, FCoreStat(PhyDef, PhyDef));
	StatsNonRegenMap.Add(EFullStats::MagicDefense, FCoreStat(MagDef, MagDef));

	BroadcastStats(StatsRegenMap, OnRegenStatsUpdated);
	BroadcastStats(StatsNonRegenMap, OnNonRegenStatsUpdated);

	LevelUpStats.Initialize();
	const UEnum* EnumPtr = StaticEnum<ELevelUpStats>();
	for (int i = 1; i < EnumPtr->NumEnums() - 1; ++i) // MAX 제외
	{
		ELevelUpStats Temp = static_cast<ELevelUpStats>(EnumPtr->GetValueByIndex(i));
		if (Temp != ELevelUpStats::MAX) OnLevelUpStatsUpdated.Broadcast(Temp, LevelUpStats.GetAllocatedPoint(Temp));
	}	
}
// ========================================================
// 기본 스탯 관련
// ========================================================
bool UStatComponent::AllocatedLevelUpStat(ELevelUpStats StatType)
{
	// 현재 경험치가 필요경험치보다 적음 -> 스탯 투자 못함.
	if (CurrentExp < RequiredExp) return false;

	CurrentExp -= RequiredExp;
	switch (StatType)	
	{
	case ELevelUpStats::Strength:
		LevelUpStats.AllocatePoint(StatType);
		UpgradeStrength();
		break;
	case ELevelUpStats::Dexterity:
		LevelUpStats.AllocatePoint(StatType);
		UpgradeDexterity();
		break;
	default:
		UE_LOG(LogTemp, Error, TEXT("UStatComponent::AllocatedLevelUpStat => 잘못된 레벨업스탯값 들어옴"));
		return false;
	}

	RequiredExp *= 1.1;
	OnLevelUpStatsUpdated.Broadcast(StatType, LevelUpStats.GetAllocatedPoint(StatType));
	OnExpChanged.Broadcast(CurrentExp);
	return true;
}
void UStatComponent::UpgradeStrength()
{
	// 힘 증가 -> 최대 체력 & 물리 공격력 증가
	if (FCoreStat* HealthStat = StatsRegenMap.Find(EFullStats::Health))
	{
		HealthStat->Max *= 1.1f; // 최대 체력 10% 증가
		HealthStat->Current = HealthStat->Max; // 레벨업 시 체력 회복
	}
	if (FCoreStat* PhyAttStat = StatsNonRegenMap.Find(EFullStats::PhysicalAttack))
	{
		PhyAttStat->Current *= 1.1f; // 물공 10% 증가
	}
	BroadcastStats(StatsRegenMap, OnRegenStatsUpdated);
	BroadcastStats(StatsNonRegenMap, OnNonRegenStatsUpdated);
}
void UStatComponent::UpgradeDexterity()
{
	// 민첩 증가 -> 최대 기력 & 공격 속도 증가
	if (FCoreStat* StaminaStat = StatsRegenMap.Find(EFullStats::Stamina))
	{
		StaminaStat->Max *= 1.1f; // 최대 스태미너 10% 증가
	}
	if (FCoreStat* AttSpeedStat = StatsNonRegenMap.Find(EFullStats::AttackSpeed))
	{
		// 0.03씩 증가 (3% 빨라짐)
		AttSpeedStat->Current += 0.03f;
	}
	BroadcastStats(StatsRegenMap, OnRegenStatsUpdated);
	BroadcastStats(StatsNonRegenMap, OnNonRegenStatsUpdated);
}
// ========================================================
// 파생 스탯 관련
// ========================================================
void UStatComponent::ChangeStatCurrent(EFullStats StatType, float DeltaCurrent)
{
	if (FCoreStat* Stat = StatsRegenMap.Find(StatType))
	{
		Stat->Current = FMath::Clamp(Stat->Current + DeltaCurrent, 0.f, Stat->Max);
		OnRegenStatChanged.Broadcast(StatType, Stat->Current, Stat->Max);
		return;
	}
	if (FCoreStat* StatNon = StatsNonRegenMap.Find(StatType))
	{
		StatNon->Current = FMath::Clamp(StatNon->Current + DeltaCurrent, 0.f, StatNon->Max);
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("ChangeStatCurrent: StatType %d not found!"), static_cast<int32>(StatType));
}
float UStatComponent::GetStatMax(EFullStats StatType) const
{
	if (const FCoreStat* Stat = StatsRegenMap.Find(StatType))
	{
		return Stat->Max;
	}
	if (const FCoreStat* StatNon = StatsNonRegenMap.Find(StatType))
	{
		return StatNon->Max;
	}
	UE_LOG(LogTemp, Warning, TEXT("GetStatMax: StatType %d not found!"), static_cast<int32>(StatType));
	return 0.f;
}
float UStatComponent::GetStatCurrent(EFullStats StatType) const
{
	if (const FCoreStat* Stat = StatsRegenMap.Find(StatType))
	{
		return Stat->Current;
	}
	if (const FCoreStat* StatNon = StatsNonRegenMap.Find(StatType))
	{
		return StatNon->Current;
	}
	UE_LOG(LogTemp, Warning, TEXT("GetStatCurrent: StatType %d not found!"), static_cast<int32>(StatType));
	return 0.f;
}
// ========================================================
// 경험치 관련
// ========================================================
void UStatComponent::AddCurrentExp(float Amount)
{
	CurrentExp += Amount;
	OnExpChanged.Broadcast(CurrentExp);
}
// ========================================================
// 타이머
// ========================================================
void UStatComponent::HandleStatRegeneration()
{
	static UEnum* EnumPtr = StaticEnum<EFullStats>();
	for (auto& Pair : StatsRegenMap) {
		FCoreStat& Stat = Pair.Value;
		// 스태미너이고, 일시정지 상태라면 회복 건너뜀
		if (Pair.Key == EFullStats::Stamina && bIsStaminaRegenPaused)
		{
			continue;
		}

		if (!FMath::IsNearlyZero(Stat.TickRate)) {
			Stat.Current = FMath::Clamp(Stat.Current + Stat.TickRate * TimerTick, 0.f, Stat.Max);
			OnRegenStatChanged.Broadcast(Pair.Key, Stat.Current, Stat.Max);
			//const FString StatName = EnumPtr
			//	? EnumPtr->GetNameStringByValue((int64)Pair.Key)
			//	: TEXT("Unknown");

			//UE_LOG(LogTemp, Log, TEXT("[REGEN] %s    %.2f / %.2f  (TickRate: %.2f)"),
			//	*StatName,
			//	Stat.Current,
			//	Stat.Max,
			//	Stat.TickRate);
		}
	}
}