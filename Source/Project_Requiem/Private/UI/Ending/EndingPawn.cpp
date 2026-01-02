#include "UI/Ending/EndingPawn.h"
#include "Kismet/GameplayStatics.h"
#include "UI/MainMenu/PRSplinePathActor.h"
#include "Engine/DirectionalLight.h"

AEndingPawn::AEndingPawn()
{
    PrimaryActorTick.bStartWithTickEnabled = false;

    DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DummyRoot"));
    RootComponent = DummyRoot;

    CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
    CameraComp->SetupAttachment(RootComponent);

    ElapsedTime = 0.0f;
}

void AEndingPawn::EndingWalkingCinematic()
{
    ElapsedTime = 0.0f;
    bIsMoving = true;
    SetActorTickEnabled(true);
    CurrentState = EEndingState::Walking;
}

void AEndingPawn::BeginPlay()
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

    if (!SunLight) {
        TArray<AActor*> FoundLights;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADirectionalLight::StaticClass(), FoundLights);

        if (FoundLights.Num() > 0) SunLight = Cast<ADirectionalLight>(FoundLights[0]);
    }
}

void AEndingPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    switch (CurrentState) {
    case EEndingState::Walking:{
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
            OnEndingWalkingFinished.Broadcast();
            CurrentState = EEndingState::None;
        }
    }
        break;
    case EEndingState::LookAtSun: {
        if (!SunLight) return;

        //FVector SunDir = -SunLight->GetActorForwardVector();
        //FVector LookTarget = GetActorLocation() + SunDir * 10000.f;

        //FRotator TargetRot = (LookTarget - GetActorLocation()).Rotation();
        //FRotator NewRot = FMath::RInterpTo(GetActorRotation(),TargetRot,DeltaTime,LookAtInterpSpeed);

        //SetActorRotation(NewRot);

        //if (GetActorRotation().Equals(TargetRot, 1.0f)) {
        //    bLookAtSun = false;
        //    SetActorTickEnabled(false);
        //}
        SetActorTickEnabled(false);
    }
        break;
    case EEndingState::None:
    case EEndingState::Finished:
        break;
    }
}

void AEndingPawn::StartLookAtSun()
{
    bLookAtSun = true;
    SetActorTickEnabled(true);
    CurrentState = EEndingState::LookAtSun;
}
