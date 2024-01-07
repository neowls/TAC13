// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/TACHPWidget.h"
#include "Components/ProgressBar.h"

UTACHPWidget::UTACHPWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	MaxHP = 0;
}

void UTACHPWidget::NativeConstruct()
{
	Super::NativeConstruct();
	HPProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PbHPBar")));
	ensure(HPProgressBar);

	
}

void UTACHPWidget::UpdateHP(uint8 NewCurrentHP)
{
	ensure(MaxHP > 0);
	if(HPProgressBar)
	{
		HPProgressBar->SetPercent(NewCurrentHP / MaxHP);
	}
}
