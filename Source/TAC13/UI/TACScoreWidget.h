// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/TACUserWidget.h"
#include "GameData/TACStruct.h"
#include "TACScoreWidget.generated.h"

/**
 * 
 */
UCLASS()
class TAC13_API UTACScoreWidget : public UTACUserWidget
{
	GENERATED_BODY()

public:
	UTACScoreWidget(const FObjectInitializer& ObjectInitializer);

	void SetPlayerScoreInfo(FPlayerScore* InPlayerScoreInfo);
	FORCEINLINE void ReleasePlayerScorePtr() { PlayerScoreInfoPtr = nullptr; };

	UFUNCTION(BlueprintPure)
	bool IsValidScoreWidget() const { return PlayerScoreInfoPtr ? true : false;}
	
protected:
	virtual void NativeConstruct() override;
	
	FPlayerScore* PlayerScoreInfoPtr;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<class UTextBlock> PlayerTextBlock;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<class UTextBlock> KillScoreTextBlock;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<class UTextBlock> DeathScoreTextBlock;

	UFUNCTION(BlueprintPure)
	FText GetPlayerNameText() const { return FText::FromString(PlayerScoreInfoPtr ? PlayerScoreInfoPtr->PlayerName : "None");	}

	UFUNCTION(BlueprintPure)
	FText GetKillScoreText() const { return FText::AsNumber(PlayerScoreInfoPtr ? PlayerScoreInfoPtr->KillScore : 0);	}

	UFUNCTION(BlueprintPure)
	FText GetDeathScoreText() const { return FText::AsNumber(PlayerScoreInfoPtr ? PlayerScoreInfoPtr->DeathScore : 0);	}
};
