// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/TACScoreWidget.h"

#include "Components/TextBlock.h"

UTACScoreWidget::UTACScoreWidget(const FObjectInitializer& ObjectInitializer)
{
	PlayerScoreInfoPtr = nullptr;
}

void UTACScoreWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
}

void UTACScoreWidget::SetPlayerScoreInfo(FPlayerScore* InPlayerScoreInfo)
{
	if(InPlayerScoreInfo)
	{
		PlayerScoreInfoPtr = InPlayerScoreInfo;
	}
}
