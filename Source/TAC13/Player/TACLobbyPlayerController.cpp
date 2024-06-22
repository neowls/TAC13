//

#include "Player/TACLobbyPlayerController.h"
#include "TAC13.h"
#include "Game/Lobby/TACLobbyGameState.h"
#include "Game/Lobby/TACLobbyPlayerState.h"
#include "UI/TACLobbyWidget.h"
#include "UI/TACUserWidget.h"

ATACLobbyPlayerController::ATACLobbyPlayerController(const FObjectInitializer& ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UTACUserWidget> LobbyWidgetRef(TEXT("/Game/_TAC/Menu/WBP_Lobby.WBP_Lobby_C"));
	if(LobbyWidgetRef.Class)
	{
		UIWidgetClass = LobbyWidgetRef.Class;
	}
	bAutoManageActiveCameraTarget = false;
}

void ATACLobbyPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	LobbyGameState = GetWorld()->GetGameState<ATACLobbyGameState>();
}

void ATACLobbyPlayerController::BeforeSetUI()
{
	Super::BeforeSetUI();
	LobbyGameState = GetWorld()->GetGameState<ATACLobbyGameState>();
}

void ATACLobbyPlayerController::SetUIWidget()
{
	Super::SetUIWidget();
	LobbyWidget = Cast<UTACLobbyWidget>(UIWidget);
	LobbyGameState->OnUpdatePlayerList.AddUObject(LobbyWidget, &UTACLobbyWidget::UpdatePlayerInfo);
	LobbyWidget->OnChangeReadyStateDelegate.AddDynamic(this, &ThisClass::Server_ChangePlayerReadyState);
	if(HasAuthority()) LobbyWidget->bIsHost = true;
	LobbyGameState->UpdatePlayerList();
}

void ATACLobbyPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
}

void ATACLobbyPlayerController::RequestUpdatePlayerInfo()
{
	if(IsLocalPlayerController()) Server_RequestUpdatePlayerList();
}

void ATACLobbyPlayerController::Server_RequestUpdatePlayerList_Implementation()
{
	if(LobbyGameState) LobbyGameState->Multicast_SendUpdatePlayerList();
}

void ATACLobbyPlayerController::Server_ChangePlayerReadyState_Implementation()
{
	GetPlayerState<ATACLobbyPlayerState>()->SetReadyStatus();
}

