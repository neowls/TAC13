// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/TACUserWidget.h"
#include "TACKillLogWidget.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class TAC13_API UTACKillLogWidget : public UTACUserWidget
{
	GENERATED_BODY()

public:
	UTACKillLogWidget(const FObjectInitializer& ObjectInitializer);

	
	
protected:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TWeakObjectPtr<UTextBlock> AttackerTextBlock;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TWeakObjectPtr<UTextBlock> WeaponNameTextBlock;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TWeakObjectPtr<UTextBlock> VictimTextBlock;

	FTimerHandle DeleteTimer;
	float DeleteDelayTime = 4.0f;

public:
	UFUNCTION(BlueprintCallable)
	virtual void UpdateKillLogText(FString InKillerName, FString InWeaponName, FString InVictimName);

	void RemoveKillLog();
};
