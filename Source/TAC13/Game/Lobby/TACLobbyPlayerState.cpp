// Fill out your copyright notice in the Description page of Project Settings.


#include "TACLobbyPlayerState.h"
#include "Interface/TACPlayerStateRequestInterface.h"
#include "Net/UnrealNetwork.h"


ATACLobbyPlayerState::ATACLobbyPlayerState()
{
	bReadyState = false;
}


void ATACLobbyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATACLobbyPlayerState, bReadyState);
}

void ATACLobbyPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ATACLobbyPlayerState::BeginPlay()
{
	Super::BeginPlay();
	if(ITACPlayerStateRequestInterface* InterfacePawn = Cast<ITACPlayerStateRequestInterface>(Owner))
	{
		InterfacePawn->RequestUpdatePlayerInfo();
	}
}

void ATACLobbyPlayerState::Destroyed()
{
	Super::Destroyed();
	if(ITACPlayerStateRequestInterface* InterfacePawn = Cast<ITACPlayerStateRequestInterface>(Owner))
	{
		InterfacePawn->RequestUpdatePlayerInfo();
	}
}

void ATACLobbyPlayerState::SetReadyStatus()
{
	bReadyState = !bReadyState;
}
