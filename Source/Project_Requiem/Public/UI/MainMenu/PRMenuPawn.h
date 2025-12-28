#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/SplineComponent.h"
#include "Camera/CameraComponent.h"
#include "PRMenuPawn.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCinematicFinished);

UCLASS()
class PROJECT_REQUIEM_API APRMenuPawn : public APawn
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable)
    FOnCinematicFinished OnCinematicFinished;

public:
    APRMenuPawn();

    UFUNCTION()
    void StartCinematic();

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
    float MoveDuration = 5.0f;

    float ElapsedTime = 0.0f;
    bool bIsMoving = true;
};