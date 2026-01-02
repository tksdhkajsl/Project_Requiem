#include "UI/MainMenu/PRSplinePathActor.h"

APRSplinePathActor::APRSplinePathActor()
{
	PrimaryActorTick.bCanEverTick = false;
	PathSpline = CreateDefaultSubobject<USplineComponent>(TEXT("PathSpline"));
	SetRootComponent(PathSpline);
}