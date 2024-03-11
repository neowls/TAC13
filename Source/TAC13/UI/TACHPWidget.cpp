// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/TACHPWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

UTACHPWidget::UTACHPWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}

void UTACHPWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UTACHPWidget::UpdateHP(uint8 NewCurrentHP)
{
	CurrentHP = NewCurrentHP;
	ensure(MaxHP > 0);

	const float HPPercent = CurrentHP * 0.01f;
	
	if(HPProgressBar.IsValid())
	{
		HPProgressBar->SetPercent(HPPercent);
	}

	if(HPText.IsValid())
	{
		HPText->SetText(FText::FromString(GetHPText()));
	}
}

FString UTACHPWidget::GetHPText()
{
	return FString::Printf(TEXT("%d"), CurrentHP);
}
