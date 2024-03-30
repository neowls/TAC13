// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/TACGameState.h"
#include "TACLobbyGameState.generated.h"



/**
 * 
 */
UCLASS()
class TAC13_API ATACLobbyGameState : public ATACGameState
{
	GENERATED_BODY()

public:
	ATACLobbyGameState();

	UPROPERTY(Transient, BlueprintReadOnly, Category=GameMode)
	TObjectPtr<class ATACLobbyGameMode> AuthTACLobbyGameMode;

	virtual void AddPlayerState(APlayerState* PlayerState) override;

	virtual void RemovePlayerState(APlayerState* PlayerState) override;
	

	void UpdatePlayerInfos();
	
	bool IsAllPlayerReady();

protected:
	
	virtual void PreInitializeComponents() override;

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	TArray<struct FLobbyPlayerInfo> PlayerInfos;

	UPROPERTY(ReplicatedUsing = OnRep_LobbyPlayerStates)
	TArray<class ATACLobbyPlayerState*> LobbyPlayerStates;
	
	UFUNCTION()
	void OnRep_LobbyPlayerStates();
};


