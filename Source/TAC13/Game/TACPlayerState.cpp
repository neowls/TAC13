// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/TACPlayerState.h"
#include "TAC13.h"
#include "Net/UnrealNetwork.h"

ATACPlayerState::ATACPlayerState()
{
	
}

void ATACPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	SetPlayerName("Player");
	TAC_LOG(LogTACNetwork, Log, TEXT("%s"), *GetPlayerName());
}

void ATACPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATACPlayerState, PlayerScore);
	
}
