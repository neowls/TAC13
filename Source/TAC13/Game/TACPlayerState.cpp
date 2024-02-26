// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/TACPlayerState.h"
#include "TAC13.h"
#include "Net/UnrealNetwork.h"

ATACPlayerState::ATACPlayerState()
{
	PlayerScore.PlayerName = "Player";
	PlayerScore.KillScore = 0;
	PlayerScore.DeathScore = 0;
}

void ATACPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ATACPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATACPlayerState, PlayerScore);
	
}

void ATACPlayerState::OnRep_PlayerScore()
{
	TAC_LOG(LogTACNetwork, Log, TEXT("Kill Score : %d"), PlayerScore.KillScore);
	TAC_LOG(LogTACNetwork, Log, TEXT("Death Score : %d"), PlayerScore.DeathScore);
}

void ATACPlayerState::AddKillScore()
{
	if(GetLocalRole() == ROLE_Authority)
	{
		PlayerScore.KillScore++;
		OnRep_PlayerScore();
	}
}

void ATACPlayerState::AddDeathScore()
{
	if(GetLocalRole() == ROLE_Authority)
	{
		PlayerScore.DeathScore++;
		OnRep_PlayerScore();
	}
}


