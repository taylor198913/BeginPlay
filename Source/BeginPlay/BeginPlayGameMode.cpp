// Copyright Epic Games, Inc. All Rights Reserved.

#include "BeginPlayGameMode.h"
#include "BeginPlayCharacter.h"
#include "MyPlayerController.h"
#include "UObject/ConstructorHelpers.h"

ABeginPlayGameMode::ABeginPlayGameMode()
{
	PlayerControllerClass = AMyPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
