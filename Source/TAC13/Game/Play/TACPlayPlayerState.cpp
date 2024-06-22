
#include "TACPlayPlayerState.h"
#include "TAC13.h"
#include "Net/UnrealNetwork.h"

ATACPlayPlayerState::ATACPlayPlayerState()
{
	PlayerScore.PlayerName = "Player";
	PlayerScore.KillScore = 0;
	PlayerScore.DeathScore = 0;
}


void ATACPlayPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATACPlayPlayerState, PlayerScore);
}

void ATACPlayPlayerState::BeginPlay()
{
	TAC_LOG(LogTACNetwork, Warning, TEXT("Start Owner : %s"), *GetName());
	Super::BeginPlay();
	PlayerScore.PlayerName = GetPlayerName();
	TAC_LOG(LogTACNetwork, Warning, TEXT("End Owner : %s"), *GetName());
}

void ATACPlayPlayerState::OnRep_PlayerScore()
{
	TAC_LOG(LogTACNetwork, Log, TEXT("Kill Score : %d"), PlayerScore.KillScore);
	TAC_LOG(LogTACNetwork, Log, TEXT("Death Score : %d"), PlayerScore.DeathScore);
}

void ATACPlayPlayerState::OnRep_PlayerName()
{
	Super::OnRep_PlayerName();
	PlayerScore.PlayerName = GetPlayerName();
}

void ATACPlayPlayerState::AddKillScore()
{
	if(GetLocalRole() == ROLE_Authority)
	{
		PlayerScore.KillScore++;
		OnRep_PlayerScore();
	}
}

void ATACPlayPlayerState::AddDeathScore()
{
	if(GetLocalRole() == ROLE_Authority)
	{
		PlayerScore.DeathScore++;
		OnRep_PlayerScore();
	}
}
