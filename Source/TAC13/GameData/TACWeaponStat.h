
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "TACWeaponStat.generated.h"

UENUM(BlueprintType)
enum class EFireMode:uint8
{
	SINGLE,
	BURST,
	FULL
};

USTRUCT()
struct FTACWeaponStat : public FTableRowBase
{
	GENERATED_BODY()

public:
	FTACWeaponStat() : Damage(0.0f), FireRate(0.0f), Recoil(0.0f), Ergonomics(0.0f), MaxAmmo(0) {};

	UPROPERTY()
	TArray<EFireMode> OwnFireMode;
	
	UPROPERTY()
	float Damage;

	UPROPERTY()
	float FireRate;

	UPROPERTY()
	float Recoil;

	UPROPERTY()
	float Ergonomics;

	UPROPERTY()
	uint8 MaxAmmo;
};
