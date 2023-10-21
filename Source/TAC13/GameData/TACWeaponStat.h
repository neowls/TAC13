
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
	//FTACWeaponStat();

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
