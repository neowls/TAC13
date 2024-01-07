// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TACUserWidget.h"
#include "TACHPWidget.generated.h"

/**
 * 
 */
UCLASS()
class TAC13_API UTACHPWidget : public UTACUserWidget
{
	GENERATED_BODY()

public:
	UTACHPWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;
	
public:
	FORCEINLINE void SetMaxHP(uint8 NewMaxHP) { MaxHP = NewMaxHP; }
	void UpdateHP(uint8 NewCurrentHP);
	
protected:
	UPROPERTY()
	TObjectPtr<class UProgressBar> HPProgressBar;
	
	UPROPERTY()
	uint8 MaxHP;
};
