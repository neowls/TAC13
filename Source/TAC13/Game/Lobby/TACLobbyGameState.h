// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TACLobbyPlayerState.h"
#include "Game/TACGameState.h"
#include "TACLobbyGameState.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnUpdateLobbyPlayerListDelegate, TArray<ATACLobbyPlayerState*>& /*InLobbyPlayerList*/);

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

	FORCEINLINE const TArray<ATACLobbyPlayerState*>& GetLobbyPlayerArray() const { return LobbyPlayerArray; }

	FOnUpdateLobbyPlayerListDelegate OnUpdatePlayerList;

	virtual void AddPlayerState(APlayerState* PlayerState) override;

	virtual void RemovePlayerState(APlayerState* PlayerState) override;
	
	UFUNCTION(BlueprintCallable)
	bool IsAllPlayerReady();

	UFUNCTION(Server, Reliable)
	void Server_RequestUpdatePlayerList();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SendUpdatePlayerList();

	UFUNCTION()
	void UpdatePlayerList();

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated)
	TArray<ATACLobbyPlayerState*> LobbyPlayerArray;
};


