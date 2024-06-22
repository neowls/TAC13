// Fill out your copyright notice in the Description page of Project Settings.


#include "TACGameMode.h"
#include "TACGameState.h"
#include "TACPlayerState.h"
#include "Character/TACLobbyPawn.h"
#include "Player/TACLobbyPlayerController.h"


ATACGameMode::ATACGameMode()
{
	DefaultPawnClass = ATACLobbyPawn::StaticClass();
	PlayerControllerClass = ATACPlayerController::StaticClass();
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
