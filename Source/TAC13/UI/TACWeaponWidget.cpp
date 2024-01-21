// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/TACWeaponWidget.h"
#include "Components/TextBlock.h"
#include "Interface/TACCharacterWidgetInterface.h"


UTACWeaponWidget::UTACWeaponWidget(const FObjectInitializer& ObjectInitializer)
{
	OwnAmmo = 0;
	CurrentAmmo = 0;
}

void UTACWeaponWidget::NativeConstruct()
{
	Super::NativeConstruct();
	OwnAmmoTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("TbOwnAmmo")));
	ensure(OwnAmmoTextBlock);
	
	CurrentAmmoTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("TbCurrentAmmo")));
	ensure(CurrentAmmoTextBlock);

	ITACCharacterWidgetInterface* CharacterWidget = Cast<ITACCharacterWidgetInterface>(OwningActor);
	if(CharacterWidget)
	{
		CharacterWidget->SetupCharacterWidget(this);
	}
}

void UTACWeaponWidget::UpdateOwnAmmo(uint8 NewOwnAmmo)
{
	if(OwnAmmoTextBlock)
	{
		OwnAmmoTextBlock->SetText(FText::FromString(BytesToString(&NewOwnAmmo, 1)));
	}
}

void UTACWeaponWidget::UpdateCurrentAmmo(uint8 NewCurrentAmmo)
{
	if(CurrentAmmoTextBlock)
	{
		CurrentAmmoTextBlock->SetText(FText::FromString(BytesToString(&NewCurrentAmmo, 1)));
	}
}
