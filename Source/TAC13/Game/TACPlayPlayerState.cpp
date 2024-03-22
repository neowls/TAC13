
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

void ATACPlayPlayerState::OnRep_PlayerScore()
{
	TAC_LOG(LogTACNetwork, Log, TEXT("Kill Score : %d"), PlayerScore.KillScore);
	TAC_LOG(LogTACNetwork, Log, TEXT("Death Score : %d"), PlayerScore.DeathScore);
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
