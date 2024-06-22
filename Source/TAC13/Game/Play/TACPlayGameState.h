// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/TACGameState.h"
#include "GameData/TACStruct.h"
#include "TACPlayGameState.generated.h"

/**
 * 
 */
UCLASS()
class TAC13_API ATACPlayGameState : public ATACGameState
{
	GENERATED_BODY()


public:
	ATACPlayGameState();
	
	UPROPERTY(ReplicatedUsing=OnRep_KillLogUpdated)
	TArray<FKillLogEntry> KillLogList;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_PlayerList)
	TArray<class ATACPlayPlayerState*> TACPlayerList;

	UPROPERTY(Transient, BlueprintReadOnly, Category=GameState)
	TObjectPtr<class ATACPlayGameMode> AuthTACPlayGameMode;
	
	UFUNCTION()
	void OnRep_KillLogUpdated();

	UFUNCTION()
	void OnRep_PlayerList();

	UFUNCTION()
	void AddKillLogEntry(const FString& KillerName, const FString& WeaponName ,const FString& VictimName);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BeginPlay() override;

	UPROPERTY()
	TObjectPtr<class ATACPlayPlayerController> FirstPlayerController;
	
};
