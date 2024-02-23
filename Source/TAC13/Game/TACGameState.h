// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TACGameMode.h"
#include "GameFramework/GameState.h"
#include "GameData/TACStruct.h"
#include "TACGameState.generated.h"

UCLASS()
class TAC13_API ATACGameState : public AGameState
{
	GENERATED_BODY()

public:
	ATACGameState();

	UPROPERTY(ReplicatedUsing=OnRep_KillLogUpdated)
	TArray<FKillLogEntry> KillLogList;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_PlayerList)
	TArray<class ATACPlayerState*> TACPlayerList;

	UPROPERTY(Transient, BlueprintReadOnly, Category=GameState)
	TObjectPtr<ATACGameMode> AuthTACGameMode;
	
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
	TObjectPtr<class ATACPlayerController> FirstPlayerController;


	
};
