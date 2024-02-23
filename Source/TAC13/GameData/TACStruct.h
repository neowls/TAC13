#pragma once

#include "CoreMinimal.h"
#include "TACStruct.generated.h"

USTRUCT(BlueprintType)
struct FPlayerScore
{
	GENERATED_BODY()

	FPlayerScore() : PlayerName("Player"), KillScore(0), DeathScore(0) {};
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString PlayerName;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 KillScore;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 DeathScore;
};

USTRUCT(BlueprintType)
struct FKillLogEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString KillerName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString VictimName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString WeaponName;
};