// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/TACGameMode.h"
#include "TACPlayGameMode.generated.h"

/**
 * 
 */
UCLASS()
class TAC13_API ATACPlayGameMode : public ATACGameMode
{
	GENERATED_BODY()

public:
	ATACPlayGameMode();

	UFUNCTION()
	void AddPlayerScore(class ATACPlayPlayerState* KillPlayer, class ATACPlayPlayerState* DeathPlayer);

	class ATACPlayGameState* GetTACGameState() const { return TACPlayGameState; }

protected:
	virtual void StartPlay() override;

	virtual void PreInitializeComponents() override;
	
	virtual void PostInitializeComponents() override;
	
	virtual void PostLogin(APlayerController* NewPlayer) override;

	UPROPERTY(Transient)
	TObjectPtr<ATACPlayGameState> TACPlayGameState;
	
	
};
