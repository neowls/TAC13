// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TACPlayerController.generated.h"

UCLASS()
class TAC13_API ATACPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ATACPlayerController();
	
protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	
};
