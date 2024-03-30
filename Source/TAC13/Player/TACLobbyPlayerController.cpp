// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/TACLobbyPlayerController.h"
#include "Game/TACLobbyGameMode.h"
#include "Game/TACLobbyPlayerState.h"
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

void ATACLobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	LobbyPlayerState = Cast<ATACLobbyPlayerState>(PlayerState);
	
	FTimerHandle RequestServerPlayerListUpdateHandle;
	GetWorldTimerManager().SetTimer(RequestServerPlayerListUpdateHandle, this , &ATACLobbyPlayerController::RequestServerPlayerListUpdate, 0.05f, false);
}


void ATACLobbyPlayerController::SetUIWidget()
{
	if(IsLocalPlayerController())
	{
		LobbyWidget = CreateWidget<UTACUserWidget>(this, UIWidgetClass);
		LobbyWidget->AddToViewport();
	}
}


void ATACLobbyPlayerController::RequestServerPlayerListUpdate()
{
	if(HasAuthority())
	{
		ATACLobbyGameMode* GM = Cast<ATACLobbyGameMode>(GetWorld()->GetAuthGameMode());
	}
	
	else
	{
		ServerRequestServerPlayerListUpdate();
	}
}

void ATACLobbyPlayerController::SetIsReadyState(bool NewReadyState)
{
	if(HasAuthority())
	{
		if(LobbyPlayerState)
			LobbyPlayerState->SetIsReady(NewReadyState);
		RequestServerPlayerListUpdate();
	}
	else
	{
		ServerSetIsReadyState(NewReadyState);
	}
}

void ATACLobbyPlayerController::ServerSetIsReadyState_Implementation(bool NewReadyState)
{
	SetIsReadyState(NewReadyState);
}

void ATACLobbyPlayerController::ServerRequestServerPlayerListUpdate_Implementation()
{
	RequestServerPlayerListUpdate();
}

void ATACLobbyPlayerController::ClientUpdatePlayerList_Implementation(const TArray<FLobbyPlayerInfo>& PlayerInfos)
{
	if(!PlayerInfos.IsEmpty())
	OnUpdatePlayerList.Broadcast(PlayerInfos);
}
