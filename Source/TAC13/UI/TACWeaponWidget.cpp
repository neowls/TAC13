// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/TACWeaponWidget.h"

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
	
}

void UTACWeaponWidget::UpdateCurrentAmmo(uint8 NewCurrentAmmo)
{
	
}
