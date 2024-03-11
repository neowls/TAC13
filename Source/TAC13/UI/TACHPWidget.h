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
	void UpdateHP(uint8 NewCurrentHP);
	FString GetHPText();
	
protected:
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TWeakObjectPtr<class UProgressBar> HPProgressBar;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TWeakObjectPtr<class UTextBlock> HPText;
	
	const uint8 MaxHP = 100;

	UPROPERTY()
	uint8 CurrentHP;
};
