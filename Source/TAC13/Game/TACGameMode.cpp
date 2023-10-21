// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/TACGameMode.h"

#include "TACGameState.h"
#include "TACPlayerState.h"

ATACGameMode::ATACGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> DefaultPawnClassRef(TEXT("/Script/Engine.Blueprint'/Game/_Dev/Character/BP_TACCharacterPlayer.BP_TACCharacterPlayer_C'"));
	if (DefaultPawnClassRef.Class)
	{
		DefaultPawnClass = DefaultPawnClassRef.Class;
	}

	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerClassRef(TEXT("/Script/TAC13.TACPlayerController"));
	if (PlayerControllerClassRef.Class)
	{
		PlayerControllerClass = PlayerControllerClassRef.Class;
	}

	GameStateClass = ATACGameState::StaticClass();
	PlayerStateClass = ATACPlayerState::StaticClass();
}

void ATACGameMode::StartPlay()
{
	Super::StartPlay();
}

void ATACGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}
