// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LockOnComponent.generated.h"

class ABaseCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_REQUIEM_API ULockOnComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULockOnComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

#pragma region 설정 변수
public:
	// 락온 감지 범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LockOn")
	float SearchRadius = 1000.0f;

	// 카메라 회전 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LockOn")
	float InterpolationSpeed = 10.0f;

	// 현재 락온 중인지 여부
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LockOn")
	bool bIsLockedOn = false;

	// 현재 타겟
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LockOn")
	TObjectPtr<AActor> CurrentTarget = nullptr;
#pragma endregion

protected:
	// [추가] 12/29, 물리 쿼리 대신 이벤트를 감지할 구체 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LockOn|Optimization")
	TObjectPtr<class USphereComponent> DetectionSphere;

	// [추가] 12/29, 현재 내 주변에 들어와 있는 적들의 목록 (캐싱)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LockOn|Optimization")
	TArray<TObjectPtr<AActor>> CandidateTargets;

	// [추가] 12/29, 감지 영역 들어옴
	UFUNCTION()
	void OnDetectionSphereBeginOverlap(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	// [추가] 12/29, 감지 영역 나감
	UFUNCTION()
	void OnDetectionSphereEndOverlap(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	// [추가] 12/29, 타겟 유효성 검사 헬퍼 함수
	bool IsValidTarget(AActor* TargetActor) const;

#pragma region 기능 함수
public:
	// 외부(PlayerCharacter)에서 입력 키를 누르면 호출할 함수
	void ToggleLockOn();

private:
	void StartLockOn();
	void EndLockOn();

	// 주변 적 찾기 알고리즘
	AActor* FindBestTarget();

	// 매 프레임 카메라 회전 및 거리 체크
	void UpdateTargetLock(float DeltaTime);
#pragma endregion

private:
	// 주인 캐릭터 캐싱
	UPROPERTY()
	TObjectPtr<ACharacter> OwnerCharacter;

	// 플레이어 컨트롤러 캐싱
	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController;
		
};
