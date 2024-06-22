//

#include "TACPlayPlayerController.h"

#include "TAC13.h"
#include "UI/TACHUDWidget.h"
#include "UI/TACUserWidget.h"

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
	TAC_LOG(LogTACNetwork, Warning, TEXT("Start"));
	Super::PostInitializeComponents();
	TAC_LOG(LogTACNetwork, Warning, TEXT("End"));
}

void ATACPlayPlayerController::BeginPlay()
{
	TAC_LOG(LogTACNetwork, Warning, TEXT("Start | %s"), IsLocalController() ? TEXT("Local") : TEXT("Remote"));
	Super::BeginPlay();;
	
	if(IsLocalController())
	{
		const FInputModeGameOnly GameOnlyInputMode;
		SetInputMode(GameOnlyInputMode);
		SetShowMouseCursor(false);
	}
	TAC_LOG(LogTACNetwork, Warning, TEXT("End | %s"), IsLocalController() ? TEXT("Local") : TEXT("Remote"));
	
}

void ATACPlayPlayerController::SetUIWidget()
{
	
}

void ATACPlayPlayerController::SetHUDWidget()
{
	TAC_LOG(LogTACNetwork, Warning, TEXT("Start | %s"), IsLocalController() ? TEXT("Local") : TEXT("Remote"));
	TACHUDWidget = CreateWidget<UTACHUDWidget>(this, TACHUDWidgetClass);
	TACHUDWidget->AddToViewport();
	TAC_LOG(LogTACNetwork, Warning, TEXT("End | %s"), IsLocalController() ? TEXT("Local") : TEXT("Remote"));
}

void ATACPlayPlayerController::OnPossess(APawn* InPawn)
{
	TAC_LOG(LogTACNetwork, Warning, TEXT("Start | %s"), IsLocalController() ? TEXT("Local") : TEXT("Remote"));
	if(IsLocalController() && !TACHUDWidget) SetHUDWidget();
	Super::OnPossess(InPawn);
	TAC_LOG(LogTACNetwork, Warning, TEXT("End | %s"), IsLocalController() ? TEXT("Local") : TEXT("Remote"));
}

void ATACPlayPlayerController::PostNetInit()
{
	TAC_LOG(LogTACNetwork, Warning, TEXT("Start | %s"), IsLocalController() ? TEXT("Local") : TEXT("Remote"));
	Super::PostNetInit();
	if(IsLocalController() && !TACHUDWidget) SetHUDWidget();
	TAC_LOG(LogTACNetwork, Warning, TEXT("End | %s"), IsLocalController() ? TEXT("Local") : TEXT("Remote"));
}

void ATACPlayPlayerController::OnRep_Owner()
{
	TAC_LOG(LogTACNetwork, Warning, TEXT("Start | %s"), IsLocalController() ? TEXT("Local") : TEXT("Remote"));
	Super::OnRep_Owner();
	TAC_LOG(LogTACNetwork, Warning, TEXT("End | %s"), IsLocalController() ? TEXT("Local") : TEXT("Remote"));
}
