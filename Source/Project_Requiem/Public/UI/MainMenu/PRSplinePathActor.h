#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "PRSplinePathActor.generated.h"

UCLASS()
class PROJECT_REQUIEM_API APRSplinePathActor : public AActor
{
	GENERATED_BODY()

public:
	APRSplinePathActor();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spline Path", meta = (AllowPrivateAccess = "true"))
	USplineComponent* PathSpline;

public:
	FORCEINLINE USplineComponent* GetPathSpline() const { return PathSpline; }
};