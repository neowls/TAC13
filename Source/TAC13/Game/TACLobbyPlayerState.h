// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/TACPlayerState.h"
#include "GameData/TACStruct.h"
#include "TACLobbyPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class TAC13_API ATACLobbyPlayerState : public ATACPlayerState
{
	GENERATED_BODY()

public:
	ATACLobbyPlayerState();

	FORCEINLINE bool GetIsReady() const { return PlayerInfo.bReadyState; }
	FORCEINLINE void SetIsReady(const bool bNewReadyState) { PlayerInfo.bReadyState = bNewReadyState; }
	FORCEINLINE FLobbyPlayerInfo GetPlayerInfo()  { return PlayerInfo; }
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BeginPlay() override;

	UPROPERTY(ReplicatedUsing = OnRep_PlayerInfo)
	FLobbyPlayerInfo PlayerInfo;
	
	UFUNCTION()
	void OnRep_PlayerInfo();
};
