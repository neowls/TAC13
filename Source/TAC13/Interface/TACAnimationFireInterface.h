// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TACAnimationFireInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTACAnimationFireInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TAC13_API ITACAnimationFireInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void FireHitCheck() = 0;
};
