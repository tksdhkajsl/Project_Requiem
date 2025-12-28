#include "UI/MainMenu/PRMenuPawn.h"
#include "Kismet/GameplayStatics.h"
#include "UI/MainMenu/PRSplinePathActor.h"

APRMenuPawn::APRMenuPawn() 
    : DummyRoot(nullptr), TargetSplineComp(nullptr), TargetSplineActor(nullptr), ElapsedTime(0.0f)
{

    PrimaryActorTick.bStartWithTickEnabled = false;

    DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DummyRoot"));
    RootComponent = DummyRoot;

    CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
    CameraComp->SetupAttachment(RootComponent);

    ElapsedTime = 0.0f;
}

void APRMenuPawn::StartCinematic()
{
    ElapsedTime = 0.0f;
    bIsMoving = true;
    SetActorTickEnabled(true);
}

void APRMenuPawn::BeginPlay()
{
	Super::BeginPlay();

    if (!TargetSplineActor) {
        TArray<AActor*> FoundSplines;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), APRSplinePathActor::StaticClass(), FoundSplines);
        if (FoundSplines.Num() > 0)
        {
            TargetSplineActor = FoundSplines[0];
        }
    }

    if (TargetSplineActor) {
        TargetSplineComp = TargetSplineActor->FindComponentByClass<USplineComponent>();
        if (TargetSplineComp)
        {
            UE_LOG(LogTemp, Warning, TEXT("Spline Component Found! Starting cinematic move."));
            FVector StartPoint = TargetSplineComp->GetLocationAtDistanceAlongSpline(0.0f, ESplineCoordinateSpace::World);
            SetActorLocation(StartPoint);
        }
    }
}

void APRMenuPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bIsMoving || !TargetSplineComp || TargetSplineComp->Duration <= 0.0f) return;
    ElapsedTime += DeltaTime;
    float Alpha = FMath::Clamp(ElapsedTime / MoveDuration, 0.0f, 1.0f);

    FVector NewLocation = TargetSplineComp->GetLocationAtTime(Alpha * TargetSplineComp->Duration, ESplineCoordinateSpace::World);
    FRotator NewRotation = TargetSplineComp->GetRotationAtTime(Alpha * TargetSplineComp->Duration, ESplineCoordinateSpace::World);

    SetActorLocation(NewLocation);
    SetActorRotation(NewRotation);

    if (Alpha >= 1.0f) {
        bIsMoving = false;
        SetActorTickEnabled(false);
        OnCinematicFinished.Broadcast();
    }
}
