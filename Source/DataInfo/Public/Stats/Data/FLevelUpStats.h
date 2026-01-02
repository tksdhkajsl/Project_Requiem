#pragma once

#include "CoreMinimal.h"
#include "Stats/Data/ELevelUpStats.h"
#include "Stats/Data/EFullStats.h"
#include "FLevelUpStats.generated.h"

/**
 * @brief 캐릭터의 레벨업 시스템을 관리하는 구조체.
 *
 * 플레이어의 레벨, 투자 가능한 총 스탯 포인트, 현재까지 투자된 스탯 포인트 정보를 관리하며
 * UI를 통해 스탯을 증가/감소할 때 필요한 모든 로직을 제공한다.
 *
 * 주요 기능:
 * - 레벨 값과 누적 투자 가능 포인트(TotalPoints) 저장
 * - 투자된 스탯 정보를 TMap 형태(ELevelUpStat → 투자량)로 저장
 * - 현재까지 투자된 총합 계산(GetTotalAllocatedPoints)
 * - 남은 포인트 자동 계산(GetRemainingPoints)
 * - 포인트 투자/회수 기능(AllocatePoint, DeallocatePoint)
 * - 초기화 함수(Initialize)를 통해 모든 스탯을 0으로 초기 설정
 *
 * 이 구조체는 UI 레벨업 창, 캐릭터 StatComponent와 연동되어
 * 플레이어의 성장 규칙을 일관성 있게 처리하는 역할을 한다.
 */
USTRUCT(BlueprintType)
struct FLevelUpStats
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<ELevelUpStats, int32> AllocatedPoints;

public:
    /** 모든 스탯 카테고리를 0으로 초기화 */
    void Initialize()
    {
        const UEnum* EnumPtr = StaticEnum<ELevelUpStats>();
        if (!EnumPtr) return;

        const int32 NumEnums = EnumPtr->NumEnums();
        AllocatedPoints.Empty();

        for (int32 Index = 0; Index < NumEnums; ++Index) {
            ELevelUpStats Stat = static_cast<ELevelUpStats>(EnumPtr->GetValueByIndex(Index));
            AllocatedPoints.Add(Stat, 0);
        }

        //for (int32 Index = 0; Index < NumEnums; ++Index) {
        //    if (!EnumPtr->HasMetaData(TEXT("Hidden"), Index)) {
        //        ELevelUpStats Stat = static_cast<ELevelUpStats>(EnumPtr->GetValueByIndex(Index));

        //        if (Stat != ELevelUpStats::MAX) {
        //            AllocatedPoints.Add(Stat, 0);
        //        }
        //    }
        //}
    }

    /** 특정 스탯에 포인트 +1 */
    void AllocatePoint(ELevelUpStats Stat)
    {
        AllocatedPoints.FindOrAdd(Stat) += 1;
    }

    /** 특정 스탯 포인트 조회 */
    int32 GetAllocatedPoint(ELevelUpStats StatID) const
    {
        if (const int32* Found = AllocatedPoints.Find(StatID))
        {
            return *Found;
        }
        return 0;
    }
};