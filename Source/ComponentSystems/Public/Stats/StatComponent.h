#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Stats/Data/EFullStats.h"
#include "Stats/Data/FCoreStat.h"
#include "Stats/Data/ELevelUpStats.h"
#include "Stats/Data/FLevelUpStats.h"

#include "StatComponent.generated.h"

#pragma region 델리게이트

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLevelUpdated, ELevelUpStats, StatType, int32, Value);

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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStatsMapUpdated, const TArray<FStatEntry>&, UpdatedStats);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnRegenStatChanged, EFullStats, StatType, float, CurrValue, float, MaxValue);

//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStatsRebuilt);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnExpChanged, float, CurrentExp);
#pragma endregion

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COMPONENTSYSTEMS_API UStatComponent : public UActorComponent
{
	GENERATED_BODY()

	
/*
 * 플레이어 캐릭터 확인 사항(변수로 만들어서 관리하세요.)
 * UpgradeStrength(),UpgradeDexterity() => 업글 배수 설정하기
 * AllocatedLevelUpStat() 중 	RequiredExp *= 1.1; 배수 설정하기
*/
	
#pragma region 변경 요망
public:
	/** 타이머 틱 부분 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "00_Setting")
	float TimerTick = 1.f;

	/** 힘 부분 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "00_Setting")
	float CurrHP = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "00_Setting")
	float MaxHP = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "00_Setting")
	float HPRegen = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "00_Setting")
	float PhyAtt = 30.f;

	/** 덱스 부분 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "00_Setting")
	float CurrST = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "00_Setting")
	float MaxST = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "00_Setting")
	float STRegen = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "00_Setting")
	float AttSpeed = 1.f;

	/** 기타 부분 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "00_Setting")
	float MagAtt = 20.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "00_Setting")
	float PhyDef = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "00_Setting")
	float MagDef = 5.f;
#pragma endregion

#pragma region 델리게이트
public:
	/** 레벨업 포인트 UI용 브로드캐스트 */
	UPROPERTY(BlueprintAssignable)
	FOnLevelUpdated OnLevelUpStatsUpdated;

	/** 스텟 올렸을 때, UI바인딩용 델리게이트*/
	UPROPERTY(BlueprintAssignable)
	FOnStatsMapUpdated OnRegenStatsUpdated;
	UPROPERTY(BlueprintAssignable)
	FOnStatsMapUpdated OnNonRegenStatsUpdated;

	/** HUD용 델리게이트*/
	UPROPERTY(BlueprintAssignable)
	FOnRegenStatChanged OnRegenStatChanged;

	UPROPERTY(BlueprintAssignable, Category = "Stats")
	FOnExpChanged OnExpChanged;
#pragma endregion

#pragma region 언리얼 생성
public:
	UStatComponent();
	/** 스텟 초기화 함수*/
	void InitializeStatsComponent();
protected:
	virtual void BeginPlay() override;
#pragma endregion

#pragma region 기본 스탯 관련
public:
	/** UI쪽에서 레벨업 스텟 올릴려고 시도*/
	UFUNCTION()
	bool AllocatedLevelUpStat(ELevelUpStats StatType);
	/** 힘 관련 세부 스탯 올리는 함수*/
	void UpgradeStrength();
	/** 덱스 관련 세부 스탯 올리는 함수*/
	void UpgradeDexterity();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLevelUpStats LevelUpStats;
#pragma endregion

#pragma region 파생 스탯 관련
public:
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

#pragma region 경험치 관련
public:
	UFUNCTION(BlueprintCallable)
	float GetCurrentExp() const { return CurrentExp; }

	UFUNCTION(BlueprintCallable)
	void AddCurrentExp(float Amount);

private:
	UPROPERTY()
	float RequiredExp = 100;

	UPROPERTY()
	float CurrentExp = 0;
#pragma endregion

#pragma region 타이머
private:
	void HandleStatRegeneration();
	UPROPERTY()
	FTimerHandle StatRegenTimerHandle;
#pragma endregion
};
