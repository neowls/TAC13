// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

	UFUNCTION()
	void SetPlayerList();

	UFUNCTION()
	void OnRep_KillLogUpdated();

	UFUNCTION()
	void AddKillLogEntry(const FString& KillerName, const FString& VictimName);

	//UFUNCTION()
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BeginPlay() override;
	
};
