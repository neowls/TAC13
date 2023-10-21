// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TACGameMode.generated.h"

/**
 * 
 */
UCLASS()
class TAC13_API ATACGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ATACGameMode();

	virtual void StartPlay() override;
	
protected:
	virtual void PostInitializeComponents() override;

protected:
};
