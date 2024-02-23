// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/TACGameMode.h"
#include "TAC13.h"
#include "TACGameState.h"
#include "TACPlayerState.h"
#include "GameData/TACGameSingleton.h"

ATACGameMode::ATACGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> DefaultPawnClassRef(TEXT("/Script/Engine.Blueprint'/Game/_TAC/Character/BP_TACCharacterPlayer.BP_TACCharacterPlayer_C'"));
	if (DefaultPawnClassRef.Class)
	{
		DefaultPawnClass = DefaultPawnClassRef.Class;
	}

	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerClassRef(TEXT("/Script/TAC13.TACPlayerController"));
	if (PlayerControllerClassRef.Class)
	{
		PlayerControllerClass = PlayerControllerClassRef.Class;
	}
	

	GameStateClass = ATACGameState::StaticClass();
	PlayerStateClass = ATACPlayerState::StaticClass();
	
}

void ATACGameMode::StartPlay()
{
	Super::StartPlay();
}

void ATACGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ATACGameMode::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	TACGameState = Cast<ATACGameState>(GameState); // 게임 스테이트를 사용자 정의 게임 스테이트로 지정
	TACGameState->AuthTACGameMode = this; // 게임 스테이트 내에 게임모드를 사용자 정의 게임모드로 지정
	UTACGameSingleton::Get().InitializeGameSystem(this, TACGameState); // 싱글톤에 게임 시스템 저장
}

void ATACGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	ATACPlayerState* NewPlayerState = Cast<ATACPlayerState>(NewPlayer->PlayerState);

	if(TACGameState && NewPlayerState)
	{
		TACGameState->TACPlayerList.Add(NewPlayerState);
		TACGameState->OnRep_PlayerList();
		TAC_LOG(LogTACNetwork, Log, TEXT("Is PlayerList Count : %d"), TACGameState->TACPlayerList.Num());
	}
	
}

void ATACGameMode::AddPlayerScore(ATACPlayerState* KillPlayerState, ATACPlayerState* DeathPlayerState)
{
	KillPlayerState->AddKillScore();
	DeathPlayerState->AddDeathScore();
}
