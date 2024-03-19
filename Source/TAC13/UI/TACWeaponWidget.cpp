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
}

void UTACWeaponWidget::UpdateOwnAmmo(uint8 NewOwnAmmo)
{
	if(OwnAmmoTextBlock.IsValid())
	{
		OwnAmmoTextBlock->SetText(FText::FromString(GetAmmoText(NewOwnAmmo)));
	}
}

void UTACWeaponWidget::UpdateCurrentAmmo(uint8 NewCurrentAmmo)
{
	if(CurrentAmmoTextBlock.IsValid())
	{
		CurrentAmmoTextBlock->SetText(FText::FromString(GetAmmoText(NewCurrentAmmo)));
	}
}

void UTACWeaponWidget::UpdateWeaponName(FString NewWeaponName)
{
	if(WeaponNameTextBlock.IsValid())
	{
		WeaponNameTextBlock->SetText(FText::FromString(NewWeaponName));
	}
}

FString UTACWeaponWidget::GetAmmoText(uint8 NewAmmo)
{
	return  FString::Printf(TEXT("%d"), NewAmmo);
}
