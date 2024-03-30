// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TACGameMode.h"
#include "TACLobbyGameMode.generated.h"




class ATACLobbyPlayerController;

UCLASS()
class TAC13_API ATACLobbyGameMode : public ATACGameMode
{
	GENERATED_BODY()

public:
	ATACLobbyGameMode();

protected:

	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void InitGameState() override;

	UPROPERTY(Transient)
	TObjectPtr<class ATACLobbyGameState> TACLobbyGameState;
	
};
