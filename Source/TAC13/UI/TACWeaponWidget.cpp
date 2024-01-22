// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/TACWeaponWidget.h"
#include "Components/TextBlock.h"

UTACWeaponWidget::UTACWeaponWidget(const FObjectInitializer& ObjectInitializer)
{
	OwnAmmo = 0;
	CurrentAmmo = 0;
}

void UTACWeaponWidget::NativeConstruct()
{
	Super::NativeConstruct();
	OwnAmmoTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("TxtOwnAmmo")));
	ensure(OwnAmmoTextBlock);
	
	CurrentAmmoTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("TxtCurrentAmmo")));
	ensure(CurrentAmmoTextBlock);

	WeaponNameTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("TxtWeaponName")));
	ensure(WeaponNameTextBlock);
	
}

void UTACWeaponWidget::UpdateOwnAmmo(uint8 NewOwnAmmo)
{
	if(OwnAmmoTextBlock)
	{
		OwnAmmoTextBlock->SetText(FText::FromString(GetAmmoText(NewOwnAmmo)));
	}
}

void UTACWeaponWidget::UpdateCurrentAmmo(uint8 NewCurrentAmmo)
{
	if(CurrentAmmoTextBlock)
	{
		CurrentAmmoTextBlock->SetText(FText::FromString(GetAmmoText(NewCurrentAmmo)));
	}
}

void UTACWeaponWidget::UpdateWeaponName(FName NewWeaponName)
{
	if(WeaponNameTextBlock)
	{
		WeaponNameTextBlock->SetText(FText::FromName(NewWeaponName));
	}
}

FString UTACWeaponWidget::GetAmmoText(uint8 NewAmmo)
{
	return  FString::Printf(TEXT("%d"), NewAmmo);
}
