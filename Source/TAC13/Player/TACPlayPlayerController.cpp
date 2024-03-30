// Fill out your copyright notice in the Description page of Project Settings.


#include "TACPlayPlayerController.h"
#include "UI/TACHUDWidget.h"

ATACPlayPlayerController::ATACPlayPlayerController()
{
	static ConstructorHelpers::FClassFinder<UTACHUDWidget> TACHUDWidgetRef(TEXT("/Game/_TAC/UI/WBP_HUD.WBP_HUD_C"));
	if(TACHUDWidgetRef.Class)
	{
		TACHUDWidgetClass = TACHUDWidgetRef.Class;
	}
}

void ATACPlayPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	const FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);
}

void ATACPlayPlayerController::BeginPlay()
{
	Super::BeginPlay();

}

void ATACPlayPlayerController::SetUIWidget()
{
	if(IsLocalPlayerController())
	{
		TACHUDWidget = CreateWidget<UTACHUDWidget>(this, TACHUDWidgetClass);
		TACHUDWidget->AddToViewport();
	}
}
