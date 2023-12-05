// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Weapon/TACWeapon.h"
#include "TACGameSingleton.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTACGameSingleton, Error, All);

UCLASS()
class TAC13_API UTACGameSingleton : public UObject
{
	GENERATED_BODY()
	
public:
	UTACGameSingleton();
	
	static UTACGameSingleton& Get();

public:
	FORCEINLINE FTACWeaponStat GetWeaponStatData(FName RowName) const { return *WeaponStatTable->FindRow<FTACWeaponStat>(RowName, "WeaponStatDataTable"); }

private:
	UPROPERTY(VisibleDefaultsOnly)
	TSoftObjectPtr<class UDataTable> WeaponStatTable;
};
