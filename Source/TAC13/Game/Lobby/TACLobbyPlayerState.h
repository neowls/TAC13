// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/TACPlayerState.h"
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

	FORCEINLINE bool GetIsReady() const { return bReadyState; }

	UFUNCTION(BlueprintCallable)
	void SetReadyStatus();

	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	virtual void Destroyed() override;

	UPROPERTY(Replicated)
	uint8 bReadyState : 1;
	
};
