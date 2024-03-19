// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/TACUserWidget.h"
#include "TACScoreBoardWidget.generated.h"

/**
 * 
 */
UCLASS()
class TAC13_API UTACScoreBoardWidget : public UTACUserWidget
{
	GENERATED_BODY()

public:
	UTACScoreBoardWidget(const FObjectInitializer& ObjectInitializer);

	UFUNCTION()
	void ClearScoreBoard();

	UFUNCTION()
	void UpdateScoreBoard();
	
	void SetPlayerList(const TArray<class ATACPlayerState*>& InArray ) { CurrentPlayerList = InArray; }

protected:
	virtual void NativeConstruct() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<class UTACUserWidget> ScoreWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TWeakObjectPtr<class UVerticalBox> ScoreVerticalBox;

	TArray<class ATACPlayerState*> CurrentPlayerList;

	UPROPERTY(VisibleAnywhere)
	int WidgetCount = 8;
	
public:
	
};
