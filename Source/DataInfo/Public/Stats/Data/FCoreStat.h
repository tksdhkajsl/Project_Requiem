#pragma once

#include "CoreMinimal.h"
#include "FCoreStat.generated.h"

/**
 * @brief 개별 스탯(Stat)의 현재값, 최대값, 초당 변화량(TickRate)을 관리하는 구조체
 *
 * HP, Stamina, Hunger 등 다양한 스탯의 공통 구조를 정의합니다.
 *
 * - `Current`: 현재 수치 (예: 현재 체력)
 * - `Max`: 최대 수치 (예: 최대 체력)
 * - `TickRate`: 초당 변화량 (예: 초당 체력 회복 또는 감소)
 *
 * 이 구조체는 여러 스탯을 관리하는 컴포넌트(예: `UStatsComponent`)에서
 * 공통 자료형으로 사용됩니다.
 */
USTRUCT(BlueprintType)
struct FCoreStat
{
    GENERATED_BODY()

    // ------------------------------------------------------------
    // 스탯 기본 구성
    // ------------------------------------------------------------

    /** 현재 수치 (예: 현재 HP, 현재 스태미나 등) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
    float Current = 0.0f;

    /** 최대 수치 (예: 최대 HP, 최대 스태미나 등) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
    float Max = 0.0f;

    /** 초당 변화량 (TickRate) — 회복 또는 감소 속도 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
    float TickRate = 0.0f;

    // ------------------------------------------------------------
    // 생성자
    // ------------------------------------------------------------

    /** 기본 생성자 (0으로 초기화) */
    FCoreStat() {}

    /**
     * @brief 커스텀 초기화 생성자
     * @param InCurrent 현재값
     * @param InMax 최대값
     * @param InTickRate 초당 변화량 (기본값: 0)
     */
    FCoreStat(float InCurrent, float InMax, float InTickRate = 0.0f)
        : Current(InCurrent), Max(InMax), TickRate(InTickRate)
    {
    }
    /*
     * @brief 클램프용
    */
    void Clamp()
    {
        Current = FMath::Clamp(Current, 0.f, Max);
    }
};