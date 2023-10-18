// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TACGameSingleton.generated.h"


UCLASS()
class TAC13_API UTACGameSingleton : public UObject
{
	GENERATED_BODY()
	
public:
	UTACGameSingleton();
	static UTACGameSingleton& Get();

	
};
