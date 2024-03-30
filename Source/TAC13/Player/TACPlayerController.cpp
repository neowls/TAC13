// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/TACPlayerController.h"
#include "UI/TACUserWidget.h"

ATACPlayerController::ATACPlayerController()
{
	static ConstructorHelpers::FClassFinder<UTACUserWidget> UIWidgetRef(TEXT("/Game/_TAC/Menu/WBP_MainMenu.WBP_MainMenu_C"));
	if(UIWidgetRef.Class)
	{
		UIWidgetClass = UIWidgetRef.Class;
	}
}


void ATACPlayerController::BeginPlay()
{
	Super::BeginPlay();
	SetUIWidget();
}

void ATACPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	const FInputModeUIOnly UIOnlyInputMode;
	SetInputMode(UIOnlyInputMode);
	SetShowMouseCursor(true);
}

void ATACPlayerController::SetUIWidget()
{
	if(IsLocalPlayerController())
	{
		UIWidget = CreateWidget<UTACUserWidget>(this, UIWidgetClass);
		UIWidget->AddToViewport();
	}
}
