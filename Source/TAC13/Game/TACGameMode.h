// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TACGameMode.generated.h"


UCLASS()
class TAC13_API ATACGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ATACGameMode();

	UFUNCTION()
	void AddPlayerScore(class ATACPlayerState* KillPlayer, class ATACPlayerState* DeathPlayer);

	class ATACGameState* GetTACGameState() const { return TACGameState; }

protected:
	virtual void StartPlay() override;
	
	virtual void PostInitializeComponents() override;

	virtual void PreInitializeComponents() override;
	
	virtual void PostLogin(APlayerController* NewPlayer) override;

	UPROPERTY(Transient)
	TObjectPtr<ATACGameState> TACGameState;
	
};