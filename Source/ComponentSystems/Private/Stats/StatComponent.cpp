#include "Stats/StatComponent.h"
#include "TimerManager.h"

UStatComponent::UStatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UStatComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeStats();
	if (!FMath::IsNearlyZero(TimerTick)) {
		GetWorld()->GetTimerManager().SetTimer(StatRegenTimerHandle, this, &UStatComponent::HandleStatRegeneration, TimerTick, true);
	}
}

//----------------초기화 관련----------------------
void UStatComponent::InitializeStatsComponent()
{

	TMap<EFullStats, FCoreStat> TempRegenMap;
	TempRegenMap.Add(EFullStats::Health, FCoreStat(CurrHP, MaxHP, HPRegen));
	TempRegenMap.Add(EFullStats::Stamina, FCoreStat(CurrST, MaxST, STRegen));

	TMap<EFullStats, FCoreStat> TempNonRegenMap;
	TempNonRegenMap.Add(EFullStats::PhysicalAttack, FCoreStat(PhyAtt, PhyAtt));
	TempNonRegenMap.Add(EFullStats::MagicAttack, FCoreStat(MagAtt, MagAtt));
	TempNonRegenMap.Add(EFullStats::PhysicalDefense, FCoreStat(PhyDef, PhyDef));
	TempNonRegenMap.Add(EFullStats::MagicDefense, FCoreStat(MagDef, MagDef));

	StatsRegenMap = TempRegenMap;
	StatsNonRegenMap = TempNonRegenMap;

	TArray<FStatEntry> StatsRegenEntries;
	for (auto& Pair : StatsRegenMap)
	{
		FStatEntry Entry;
		Entry.StatType = Pair.Key;
		Entry.StatValue = Pair.Value;
		StatsRegenEntries.Add(Entry);
	}
	OnRegenStatsUpdated.Broadcast(StatsRegenEntries);

	TArray<FStatEntry> StatsNonRegenEntries;
	for (auto& Pair : StatsNonRegenMap)
	{
		FStatEntry Entry;
		Entry.StatType = Pair.Key;
		Entry.StatValue = Pair.Value;
		StatsNonRegenEntries.Add(Entry);
	}
	OnRegenStatsUpdated.Broadcast(StatsNonRegenEntries);

}
//----------------경험치 관련----------------------
void UStatComponent::AddCurrentExp(float Amount)
{
	CurrentExp += Amount;
}
//----------------스탯 관련----------------------
void UStatComponent::InitializeStats()
{
	MapRegenStats();
	MapNonRegenStats();
}
void UStatComponent::MapRegenStats()
{
	StatsRegenMap.Empty();
	StatsRegenMap.Add(EFullStats::Health, FCoreStat(CurrHP, MaxHP, HPRegen));

	StatsRegenMap.Add(EFullStats::Stamina, FCoreStat(CurrST, MaxST, STRegen));
}
void UStatComponent::MapNonRegenStats()
{
	StatsNonRegenMap.Empty();

	StatsNonRegenMap.Add(EFullStats::PhysicalAttack, FCoreStat(PhyAtt, PhyAtt, 0.f));
	StatsNonRegenMap.Add(EFullStats::MagicAttack, FCoreStat(MagAtt, MagAtt, 0.f));
	StatsNonRegenMap.Add(EFullStats::PhysicalDefense, FCoreStat(PhyDef, PhyDef, 0.f));
	StatsNonRegenMap.Add(EFullStats::MagicDefense, FCoreStat(MagDef, MagDef, 0.f));
}
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
		// 필요하면 NonRegen 전용 델리게이트 호출
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

//----------------타이머----------------------
void UStatComponent::HandleStatRegeneration()
{

	static UEnum* EnumPtr = StaticEnum<EFullStats>();

	for (auto& Pair : StatsRegenMap)
	{
		FCoreStat& Stat = Pair.Value;
		if (!FMath::IsNearlyZero(Stat.TickRate))
		{
			Stat.Current = FMath::Clamp(Stat.Current + Stat.TickRate * TimerTick, 0.f, Stat.Max);
			OnRegenStatChanged.Broadcast(Pair.Key, Stat.Current, Stat.Max);

			const FString StatName = EnumPtr
				? EnumPtr->GetNameStringByValue((int64)Pair.Key)
				: TEXT("Unknown");

			UE_LOG(LogTemp, Log, TEXT("[REGEN] %s → %.2f / %.2f  (TickRate: %.2f)"),
				*StatName,
				Stat.Current,
				Stat.Max,
				Stat.TickRate);
		}
	}

	//for (auto& Pair : StatsRegenMap)
	//{
	//	FCoreStat& Stat = Pair.Value;
	//	if (!FMath::IsNearlyZero(Stat.TickRate))
	//	{
	//		Stat.Current = FMath::Clamp(Stat.Current + Stat.TickRate * TimerTick, 0.f, Stat.Max);
	//		OnRegenStatChanged.Broadcast(Pair.Key, Stat.Current, Stat.Max);
	//	}
	//}
}