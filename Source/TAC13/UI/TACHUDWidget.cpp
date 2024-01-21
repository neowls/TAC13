// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/TACHUDWidget.h"

#include "Interface/TACCharacterHUDInterface.h"
#include "TACHPWidget.h"
#include "TACWeaponWidget.h"

UTACHUDWidget::UTACHUDWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}

void UTACHUDWidget::UpdateCurrentAmmo(uint8 NewCurrentAmmo)
{
	WeaponStat->UpdateCurrentAmmo(NewCurrentAmmo);
}

void UTACHUDWidget::UpdateOwnAmmo(uint8 NewOwnAmmo)
{
	WeaponStat->UpdateOwnAmmo(NewOwnAmmo);
}

void UTACHUDWidget::UpdateHPBar(uint8 NewCurrentHP)
{
		
	HPBar->UpdateHP(NewCurrentHP);
}

void UTACHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HPBar = Cast<UTACHPWidget>(GetWidgetFromName(TEXT("WidgetHP")));
	ensure(HPBar);
	
	WeaponStat = Cast<UTACWeaponWidget>(GetWidgetFromName(TEXT("WidgetWeapon")));
	ensure(WeaponStat);

	ITACCharacterHUDInterface* HUDPawn = Cast<ITACCharacterHUDInterface>(GetOwningPlayerPawn());
	if(HUDPawn)
	{
		HUDPawn->SetupHUDWidget(this);
	}
}
