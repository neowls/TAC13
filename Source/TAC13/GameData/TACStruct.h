#pragma once

#include "CoreMinimal.h"

USTRUCT(BlueprintType)
struct FTACPlayerScore
{
	GENERATED_BODY()

public:
	//FTACKillLog() : Damage(0.0f), FireRate(0.0f), Recoil(0.0f), Ergonomics(0.0f), MaxAmmo(0) {};
	FTACPlayerScore();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int8 KillScore;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int8 DeathScore;
};

USTRUCT(BlueprintType)
struct FKillLogEntry
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString KillerName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString VictimName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString WeaponName;
};