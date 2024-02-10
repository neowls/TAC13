// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TACHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class TAC13_API UTACHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UTACHUDWidget(const FObjectInitializer& ObjectInitializer);

public:
	void UpdateCurrentAmmo(uint8 NewCurrentAmmo);
	void UpdateOwnAmmo(uint8 NewOwnAmmo);
	void UpdateWeaponName(FName NewWeaponName);
	void UpdateHPBar(uint8 NewCurrentHP);
	void UpdateKillLog(FString InAttacker, FString InWeaponName, FString InVictim);
	void ScoreBoardOnOff(uint8 bIsScoreBoardOn);
	
	
protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY()
	TObjectPtr<class UTACHPWidget> HPBar;

	UPROPERTY()
	TObjectPtr<class UTACWeaponWidget> WeaponStat;
	
	UPROPERTY()
	TObjectPtr<class UVerticalBox> KillLogBox;

	UPROPERTY()
	TObjectPtr<class UTACScoreBoardWidget> ScoreBoard;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<class UUserWidget> KillLogClass;
	
	float RemoveDelayTime = 3.0f;
};
