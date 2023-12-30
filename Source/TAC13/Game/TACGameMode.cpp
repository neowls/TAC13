// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/TACGameMode.h"

#include "TACGameState.h"
#include "TACPlayerState.h"
#include "Player/TACHUDBase.h"

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

	static ConstructorHelpers::FClassFinder<ATACHUDBase> HUDClassRef(TEXT("/Script/Engine.Blueprint'/Game/_TAC/UI/BP_HUD.BP_HUD_C'"));
	if (HUDClassRef.Class)
	{
		HUDClass = HUDClassRef.Class;
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
