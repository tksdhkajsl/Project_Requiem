#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StageGate.generated.h"

/**
 * 스테이지 타입 Enum
 */
UENUM(BlueprintType)
enum class EStageType : uint8
{
	Stage1		UMETA(DisplayName = "Stage 1"),
	Stage2		UMETA(DisplayName = "Stage 2"),
	BossRoom	UMETA(DisplayName = "Boss Room")
};

/**
 * Stage 진입 Delegate
 * StageGateActor -> Player
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStageEntered, EStageType, NewStage);

/**
 * 스테이지 경계용 Gate Actor
 * - 레벨에 직접 배치
 * - Player가 Overlap 하면 Stage 변경 이벤트만 발생
 */
UCLASS()
class PROJECT_REQUIEM_API AStageGate : public AActor
{
	GENERATED_BODY()

public:
	AStageGate();

protected:
	virtual void BeginPlay() override;

	/** Overlap 처리 */
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

public:
	/** ===================== Components ===================== */

	/** 루트 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StageGate")
	USceneComponent* Root;

	/** 트리거 박스 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StageGate")
	class UBoxComponent* TriggerBox;

	/** ===================== Settings ===================== */

	/** 이 Gate에 진입했을 때 설정될 Stage */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "StageGate")
	EStageType EnterStage = EStageType::Stage1;

	/** ===================== Delegate ===================== */

	/** Player에게 Stage 진입을 알림 */
	UPROPERTY(BlueprintAssignable, Category = "StageGate")
	FOnStageEntered OnStageEntered;
};
