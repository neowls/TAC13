// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/TACLobbyGameMode.h"

#include "TAC13.h"
#include "TACLobbyGameState.h"
#include "TACLobbyPlayerState.h"
#include "Character/TACLobbyPawn.h"
#include "Player/TACLobbyPlayerController.h"

ATACLobbyGameMode::ATACLobbyGameMode()
{
	DefaultPawnClass = ATACLobbyPawn::StaticClass();
	PlayerControllerClass = ATACLobbyPlayerController::StaticClass();
	GameStateClass = ATACLobbyGameState::StaticClass();
	PlayerStateClass = ATACLobbyPlayerState::StaticClass();
}

void ATACLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	ATACLobbyPlayerController* JoinPlayer = Cast<ATACLobbyPlayerController>(NewPlayer);
}

void ATACLobbyGameMode::InitGameState()
{
	Super::InitGameState();
	TACLobbyGameState = GetGameState<ATACLobbyGameState>();
	TACLobbyGameState->AuthTACLobbyGameMode = this;
	
}

/*
void ATACLobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	ATACLobbyPlayerController* LobbyPlayer = Cast<ATACLobbyPlayerController>(Exiting);
	if(LobbyPlayer)
	{
		ConnectedPlayers.Remove(LobbyPlayer);
		UpdatePlayerInfos();
	}
}
*/
