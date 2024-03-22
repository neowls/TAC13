// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TACGameMode.generated.h"


class ATACGameState;

UCLASS()
class TAC13_API ATACGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ATACGameMode();

protected:
	virtual void StartPlay() override;
	
	virtual void PostInitializeComponents() override;
	
};