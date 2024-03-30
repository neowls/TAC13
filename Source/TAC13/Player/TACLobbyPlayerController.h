// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/TACPlayerController.h"
#include "GameData/TACStruct.h"
#include "TACLobbyPlayerController.generated.h"

class ATACLobbyPlayerState;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdatePlayerList, TArray<FLobbyPlayerInfo>, PlayerInfos);

UCLASS()
class TAC13_API ATACLobbyPlayerController : public ATACPlayerController
{
	GENERATED_BODY()
	
	ATACLobbyPlayerController(const FObjectInitializer& ObjectInitializer);
	
public:

	virtual void BeginPlay() override;

	virtual void SetUIWidget() override;
	
	FOnUpdatePlayerList OnUpdatePlayerList;
	
	TObjectPtr<UTACUserWidget> LobbyWidget;

	UFUNCTION(Client, Reliable)
	void ClientUpdatePlayerList(const TArray<FLobbyPlayerInfo>& PlayerInfos);

	UFUNCTION(Server, Reliable)
	void ServerRequestServerPlayerListUpdate();

	void RequestServerPlayerListUpdate();

	UFUNCTION(BlueprintCallable)
	void SetIsReadyState(bool NewReadyState);

	UFUNCTION(Server, Reliable)
	void ServerSetIsReadyState(bool NewReadyState);

protected:
	TObjectPtr<ATACLobbyPlayerState> LobbyPlayerState;
	
};
