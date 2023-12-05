// Fill out your copyright notice in the Description page of Project Settings.

#include "GameData/TACGameSingleton.h"

DEFINE_LOG_CATEGORY(LogTACGameSingleton);

UTACGameSingleton::UTACGameSingleton()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> DataTableRef(TEXT("/Script/Engine.DataTable'/Game/_TAC/DataTable/DT_Weapons.DT_Weapons'"));
	if(nullptr != DataTableRef.Object)
	{
		WeaponStatTable = DataTableRef.Object;
		check(WeaponStatTable->GetRowMap().Num() > 0)
	}
}

UTACGameSingleton& UTACGameSingleton::Get()
{
	if(UTACGameSingleton* Singleton = CastChecked<UTACGameSingleton>(GEngine->GameSingleton))
	{
		return *Singleton;
	}
	UE_LOG(LogTACGameSingleton, Error, TEXT("Invalid Game Singleton"));
	return *NewObject<UTACGameSingleton>();
}
