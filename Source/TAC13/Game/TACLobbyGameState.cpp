// Fill out your copyright notice in the Description page of Project Settings.


#include "TACLobbyGameState.h"
#include "TACLobbyPlayerState.h"
#include "TAC13.h"
#include "Net/UnrealNetwork.h"
#include "Player/TACLobbyPlayerController.h"

ATACLobbyGameState::ATACLobbyGameState()
{
	
}


void ATACLobbyGameState::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	
}

void ATACLobbyGameState::BeginPlay()
{
	Super::BeginPlay();
	UpdatePlayerInfos();
}

void ATACLobbyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass,LobbyPlayerStates);
}


void ATACLobbyGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);
	LobbyPlayerStates.AddUnique(Cast<ATACLobbyPlayerState>(PlayerState));
	TAC_LOG(LogTACNetwork, Log, TEXT("Lobby Player State Num : %d"), LobbyPlayerStates.Num());
}

void ATACLobbyGameState::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);
	LobbyPlayerStates.Remove(Cast<ATACLobbyPlayerState>(PlayerState));
	TAC_LOG(LogTACNetwork, Log, TEXT("Lobby Player State Num : %d"), LobbyPlayerStates.Num());
	
}


void ATACLobbyGameState::OnRep_LobbyPlayerStates()
{

}


void ATACLobbyGameState::UpdatePlayerInfos()
{
	PlayerInfos.Empty();
	TAC_LOG(LogTACNetwork, Log, TEXT("Lobby Player States Is Empty : %d"), LobbyPlayerStates.IsEmpty());
	for(auto it : LobbyPlayerStates)
	{
		PlayerInfos.Add(it->GetPlayerInfo());
	}
}

bool ATACLobbyGameState::IsAllPlayerReady()
	{
	for (const auto* LobbyPlayerState : LobbyPlayerStates)
	{
		if(LobbyPlayerState)
		{
			if(!LobbyPlayerState->GetIsReady()) return false;
		}
	}
	return true;
}
