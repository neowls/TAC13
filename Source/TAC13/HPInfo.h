// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "HPInfo.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHPInfoOnChangedSignature, float, InCurrentHP, float, InMaxHP);

/**
 * 
 */
UCLASS()
class TAC13_API UHPInfo : public UObject
{
	GENERATED_BODY()

public:
	UHPInfo();

	FHPInfoOnChangedSignature OnChanged;

	void ChangeHPInfo(const float InCurrentHP, const float InMaxHP);

private:
	
};
