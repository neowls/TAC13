// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/TACGameMode.h"
#include "Game/TACGameState.h"
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
	FORCEINLINE FTACWeaponInfo GetWeaponInfoData(const FName RowName) const { return *WeaponInfoTable->FindRow<FTACWeaponInfo>(RowName, "WeaponInfoDataTable"); }
	FORCEINLINE FTACAttachment GetAttachmentInfo(const FName RowName) const { return *AttachmentInfoTable->FindRow<FTACAttachment>(RowName, "AttachmentInfoDataTable"); }
	void InitializeGameSystem(ATACGameMode* InGameMode, ATACGameState* InGameState);

	ATACGameMode* GetTACGameMode() const { return TACGameMode; }
	ATACGameState* GetTACGameState() const { return TACGameState; }

private:
	UPROPERTY(VisibleDefaultsOnly)
	TSoftObjectPtr<class UDataTable> WeaponInfoTable;

	UPROPERTY(VisibleDefaultsOnly)
	TSoftObjectPtr<class UDataTable> AttachmentInfoTable;

	UPROPERTY(Transient)
	ATACGameMode* TACGameMode;

	UPROPERTY(Transient)
	ATACGameState* TACGameState;
	
};
