// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/TACHPWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Interface/TACCharacterWidgetInterface.h"

UTACHPWidget::UTACHPWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}

void UTACHPWidget::NativeConstruct()
{
	Super::NativeConstruct();
	HPProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PbHPBar")));
	ensure(HPProgressBar);

	HPText = Cast<UTextBlock>(GetWidgetFromName(TEXT("TxtHP")));
	ensure(HPText);

	ITACCharacterWidgetInterface* CharacterWidget = Cast<ITACCharacterWidgetInterface>(OwningActor);
	if(CharacterWidget)
	{
		CharacterWidget->SetupCharacterWidget(this);
	}
}

void UTACHPWidget::UpdateHP(uint8 NewCurrentHP)
{
	CurrentHP = NewCurrentHP;
	ensure(MaxHP > 0);

	const float HPPercent = CurrentHP * 0.01f;
	
	if(HPProgressBar)
	{
		HPProgressBar->SetPercent(HPPercent);
	}

	if(HPText)
	{
		HPText->SetText(FText::FromString(GetHPText()));
	}
}

FString UTACHPWidget::GetHPText()
{
	return FString::Printf(TEXT("%d"), CurrentHP);
}
