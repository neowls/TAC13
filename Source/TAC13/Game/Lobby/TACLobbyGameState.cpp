// Fill out your copyright notice in the Description page of Project Settings.


#include "TACLobbyGameState.h"

#include "TAC13.h"
#include "TACLobbyPlayerState.h"
#include "Net/UnrealNetwork.h"


ATACLobbyGameState::ATACLobbyGameState()
{

}

void ATACLobbyGameState::BeginPlay()
{
	Super::BeginPlay();
}

void ATACLobbyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, LobbyPlayerArray);
}


void ATACLobbyGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);
	if(HasAuthority())
	{
		LobbyPlayerArray.AddUnique(Cast<ATACLobbyPlayerState>(PlayerState));
	}
}

void ATACLobbyGameState::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);
	if(HasAuthority())
	{
		LobbyPlayerArray.Remove(Cast<ATACLobbyPlayerState>(PlayerState));
	}
}

bool ATACLobbyGameState::IsAllPlayerReady()
{
	if(!LobbyPlayerArray.IsEmpty())
	{
		for (const auto iter : LobbyPlayerArray)
		{
			if(iter && !iter->GetIsReady()) return false;
		}		
	}
	return true;
	
}

void ATACLobbyGameState::Server_RequestUpdatePlayerList_Implementation()
{
	Multicast_SendUpdatePlayerList();
}

void ATACLobbyGameState::Multicast_SendUpdatePlayerList_Implementation()
{
	UpdatePlayerList();
}

void ATACLobbyGameState::UpdatePlayerList()
{
	OnUpdatePlayerList.Broadcast(LobbyPlayerArray);
}
