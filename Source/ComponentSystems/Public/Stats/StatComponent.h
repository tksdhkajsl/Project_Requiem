#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Stats/Data/EFullStats.h"
#include "Stats/Data/FCoreStat.h"

#include "StatComponent.generated.h"

/** TMap못보내서 만드는 구조체*/
USTRUCT(BlueprintType)
struct FStatEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	EFullStats StatType = EFullStats::None;

	UPROPERTY(BlueprintReadOnly)
	FCoreStat StatValue;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnRegenStatChanged, EFullStats, StatType, float, CurrValue, float, MaxValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStatsMapUpdated, const TArray<FStatEntry>&, UpdatedStats);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStatsRebuilt);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnExpChanged, float, CurrentExp);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COMPONENTSYSTEMS_API UStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

#pragma region 델리게이트
public:
	/** 리젠 스탯용 델리게이트*/
	UPROPERTY(BlueprintAssignable)
	FOnRegenStatChanged OnRegenStatChanged;
	/** 스텟 올렸을 때, UI바인딩용 델리게이트*/
	UPROPERTY(BlueprintAssignable)
	FOnStatsMapUpdated OnRegenStatsUpdated;

	/** 스탯 투자 성공시 던지는 델리게이트*/
	UPROPERTY(BlueprintAssignable)
	FOnStatsRebuilt OnStatsRebuilt;
	/** 현재 경험치/ Max경험치용 델리게이트*/
	UPROPERTY(BlueprintAssignable)
	FOnExpChanged OnExpChanged;
#pragma endregion

#pragma region 변경 요망
public:
	/** 타이머 틱 부분 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "00_Setting")
	float TimerTick = 1.f;

	/** 체력 부분 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "00_Setting")
	float CurrHP = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "00_Setting")
	float MaxHP = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "00_Setting")
	float HPRegen = 0.f;
	/** 스태미나 부분 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "00_Setting")
	float CurrST = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "00_Setting")
	float MaxST = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "00_Setting")
	float STRegen = 1.f;

	/** 물공,마공,물방,마방 (필요시) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "00_Setting")
	float PhyAtt = 30.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "00_Setting")
	float MagAtt = 20.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "00_Setting")
	float PhyDef = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "00_Setting")
	float MagDef = 5.f;
#pragma endregion


#pragma region 초기화 관련
public:
	/** 스텟 초기화 함수*/
	void InitializeStatsComponent();
#pragma endregion


#pragma region 경험치 관련
public:
	UFUNCTION(BlueprintCallable)
	float GetCurrentExp() const { return CurrentExp; }

	UFUNCTION(BlueprintCallable)
	void AddCurrentExp(float Amount);

private:

	UPROPERTY()
	float CurrentExp = 0;
#pragma endregion

#pragma region 스탯 관련
public:
	/**
	 * @brief 스탯 초기화 함수
	 */
	void InitializeStats();
	/**
	 * @brief 리젠되는 스탯을 맵핑하는 함수
	 */
	void MapRegenStats();
	/**
	 * @brief 리젠안되는 스탯을 맵핑하는 함수
	 */
	void MapNonRegenStats();

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void ChangeStatCurrent(EFullStats StatType, float DeltaCurrent);
	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetStatMax(EFullStats StatType) const;
	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetStatCurrent(EFullStats StatType) const;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TMap<EFullStats, FCoreStat> StatsRegenMap;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TMap<EFullStats, FCoreStat> StatsNonRegenMap;
#pragma endregion

#pragma region 타이머
private:
	void HandleStatRegeneration();
	UPROPERTY()
	FTimerHandle StatRegenTimerHandle;
#pragma endregion
};
