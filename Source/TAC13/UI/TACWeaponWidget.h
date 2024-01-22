// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/TACUserWidget.h"
#include "TACWeaponWidget.generated.h"

/**
 * 
 */
UCLASS()
class TAC13_API UTACWeaponWidget : public UTACUserWidget
{
	GENERATED_BODY()

public:
	UTACWeaponWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:

	void UpdateOwnAmmo(uint8 NewOwnAmmo);
	void UpdateCurrentAmmo(uint8 NewCurrentAmmo);
	void UpdateWeaponName(FName NewWeaponName);

	FString GetAmmoText(uint8 NewAmmo);

protected:
	UPROPERTY()
	TObjectPtr<class UTextBlock> OwnAmmoTextBlock;

	UPROPERTY()
	TObjectPtr<class UTextBlock> CurrentAmmoTextBlock;

	UPROPERTY()
	TObjectPtr<class UTextBlock> WeaponNameTextBlock;
	
	UPROPERTY()
	uint8 CurrentAmmo;

	UPROPERTY()
	uint8 OwnAmmo;
};
