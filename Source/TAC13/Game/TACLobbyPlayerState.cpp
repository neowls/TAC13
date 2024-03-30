// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/TACLobbyPlayerState.h"
#include "Net/UnrealNetwork.h"


ATACLobbyPlayerState::ATACLobbyPlayerState()
{
	
}

void ATACLobbyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATACLobbyPlayerState, PlayerInfo);
}

void ATACLobbyPlayerState::BeginPlay()
{
	Super::BeginPlay();
	PlayerInfo.PlayerName = GetPlayerName();
}

void ATACLobbyPlayerState::OnRep_PlayerInfo()
{
	
}
