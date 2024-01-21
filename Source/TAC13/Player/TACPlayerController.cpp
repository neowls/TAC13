// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/TACPlayerController.h"
#include "UI/TACHUDWidget.h"

ATACPlayerController::ATACPlayerController()
{
	static ConstructorHelpers::FClassFinder<UTACHUDWidget> TACHUDWidgetRef(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/_TAC/UI/WBP_HUD.WBP_HUD_C'"));
	if(TACHUDWidgetRef.Class)
	{
		TACHUDWidgetClass = TACHUDWidgetRef.Class;
	}
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
	if(IsLocalPlayerController())
	{
		TACHUDWidget = CreateWidget<UTACHUDWidget>(this, TACHUDWidgetClass);
		if(TACHUDWidget)
		{
			TACHUDWidget->AddToViewport();
		}
	}
}
