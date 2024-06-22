//

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
	BeforeSetUI();
	if(IsLocalController())
	{
		const FInputModeUIOnly UIOnlyInputMode;
		SetInputMode(UIOnlyInputMode);
		SetShowMouseCursor(true);
		SetUIWidget();		
	}
}

void ATACPlayerController::BeforeSetUI()
{
	
}

void ATACPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ATACPlayerController::SetUIWidget()
{
	UIWidget = CreateWidget<UTACUserWidget>(this, UIWidgetClass);
	UIWidget->AddToViewport();
}

void ATACPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

}
