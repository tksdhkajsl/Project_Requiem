// Copyright Epic Games, Inc. All Rights Reserved.

#include "Project_RequiemGameMode.h"
#include "Project_RequiemCharacter.h"
#include "UObject/ConstructorHelpers.h"

AProject_RequiemGameMode::AProject_RequiemGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
