#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/SplineComponent.h"
#include "Camera/CameraComponent.h"
#include "EndingPawn.generated.h"

class ADirectionalLight;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndingWalkingFinished);

enum class EEndingState
{
    None,
    Walking,
    LookAtSun,
    Finished
};

UCLASS()
class PROJECT_REQUIEM_API AEndingPawn : public APawn
{
	GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable)
    FOnEndingWalkingFinished OnEndingWalkingFinished;
	
private:
    EEndingState CurrentState = EEndingState::None;

public:
	AEndingPawn();

    UFUNCTION()
    void EndingWalkingCinematic();

    UPROPERTY(VisibleAnywhere, Category = "Cinematic")
    USceneComponent* DummyRoot = nullptr;
    UPROPERTY(EditAnywhere, Category = "Cinematic Path")
    USplineComponent* TargetSplineComp = nullptr;
    UPROPERTY(EditAnywhere, Category = "Cinematic Path")
    class AActor* TargetSplineActor = nullptr;
protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

private:
    UPROPERTY(VisibleAnywhere)
    UCameraComponent* CameraComp;


    UPROPERTY(EditAnywhere, Category = "Cinematic")
    float MoveDuration = 10.0f;

    float ElapsedTime = 0.0f;
    bool bIsMoving = true;

#pragma region 태양 바라보는 행위
public:
    void StartLookAtSun();
    UPROPERTY(EditAnywhere, Category = "Cinematic|Sun")
    ADirectionalLight* SunLight;

    bool bLookAtSun = false;

    UPROPERTY(EditAnywhere, Category = "Sun")
    float LookAtInterpSpeed = 2.5f;
#pragma endregion

};
