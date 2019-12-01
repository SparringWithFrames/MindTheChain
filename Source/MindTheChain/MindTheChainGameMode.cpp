// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "MindTheChainGameMode.h"
#include "MindTheChainHUD.h"
#include "MindTheChainCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMindTheChainGameMode::AMindTheChainGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AMindTheChainHUD::StaticClass();
}
