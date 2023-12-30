// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TACHUDBase.generated.h"

/**
 * 
 */
UCLASS()
class TAC13_API ATACHUDBase : public AHUD
{
	GENERATED_BODY()

public:
	ATACHUDBase();

	UFUNCTION(BlueprintCallable)
	void GetChangedHP(const float CurrentHP, const float MaxHP);
};
