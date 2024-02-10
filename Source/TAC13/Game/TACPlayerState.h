// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "GameData/TACStruct.h"
#include "TACPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class TAC13_API ATACPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ATACPlayerState();

	virtual void PostInitializeComponents() override;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated)
	FTACPlayerScore PlayerScore;
};
