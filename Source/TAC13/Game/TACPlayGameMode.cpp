// Fill out your copyright notice in the Description page of Project Settings.


#include "TACPlayGameMode.h"
#include "TAC13.h"
#include "TACPlayGameState.h"
#include "TACPlayPlayerState.h"
#include "GameData/TACGameSingleton.h"
#include "Player/TACPlayPlayerController.h"

ATACPlayGameMode::ATACPlayGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> DefaultPawnClassRef(TEXT("/Script/Engine.Blueprint'/Game/_TAC/Character/BP_TACCharacterPlayer.BP_TACCharacterPlayer_C'"));
	if (DefaultPawnClassRef.Class)
	{
		DefaultPawnClass = DefaultPawnClassRef.Class;
	}
	
	PlayerControllerClass = ATACPlayPlayerController::StaticClass();
	GameStateClass = ATACPlayGameState::StaticClass();
	PlayerStateClass = ATACPlayPlayerState::StaticClass();
}


void ATACPlayGameMode::StartPlay()
{
	Super::StartPlay();
	
}

void ATACPlayGameMode::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	TACPlayGameState = Cast<ATACPlayGameState>(GameState); // 게임 스테이트를 사용자 정의 게임 스테이트로 지정
	TACPlayGameState->AuthTACPlayGameMode = this; // 게임 스테이트 내에 게임모드를 사용자 정의 게임모드로 지정
}


void ATACPlayGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
}


void ATACPlayGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	ATACPlayPlayerState* NewPlayerState = Cast<ATACPlayPlayerState>(NewPlayer->PlayerState);

	if(TACPlayGameState && NewPlayerState)
	{
		TACPlayGameState->TACPlayerList.Add(NewPlayerState);
		TACPlayGameState->OnRep_PlayerList();
		TAC_LOG(LogTACNetwork, Log, TEXT("Is PlayerList Count : %d"), TACPlayGameState->TACPlayerList.Num());
	}
}

void ATACPlayGameMode::AddPlayerScore(ATACPlayPlayerState* KillPlayerState, ATACPlayPlayerState* DeathPlayerState)
{
	KillPlayerState->AddKillScore();
	DeathPlayerState->AddDeathScore();
}
