// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/TACPlayerController.h"

ATACPlayerController::ATACPlayerController()
{
}

void ATACPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ATACPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	const FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);
}
