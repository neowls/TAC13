// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/TACPlayerController.h"
#include "TACPlayPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class TAC13_API ATACPlayPlayerController : public ATACPlayerController
{
	GENERATED_BODY()

public:

	ATACPlayPlayerController();

	FORCEINLINE class UTACHUDWidget* GetTACHUDWidget() { return TACHUDWidget; }

protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void SetUIWidget() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	TSubclassOf<class UTACHUDWidget> TACHUDWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = HUD)
	TObjectPtr<class UTACHUDWidget> TACHUDWidget;
};
