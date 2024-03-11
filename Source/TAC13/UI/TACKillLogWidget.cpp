// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/TACKillLogWidget.h"

#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"

UTACKillLogWidget::UTACKillLogWidget(const FObjectInitializer& ObjectInitializer)
{
	
}

void UTACKillLogWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
}

void UTACKillLogWidget::UpdateKillLogText(FString InKillerName, FString InWeaponName, FString InVictimName)
{
	AttackerTextBlock->SetText(FText::FromString(InKillerName));
	WeaponNameTextBlock->SetText(FText::FromString(InWeaponName));
	VictimTextBlock->SetText(FText::FromString(InVictimName));
	
	GetWorld()->GetTimerManager().SetTimer(DeleteTimer, this, &UTACKillLogWidget::RemoveKillLog, DeleteDelayTime, false);
}

void UTACKillLogWidget::RemoveKillLog()
{
	GetParent()->RemoveChild(this);
	RemoveFromParent();
}
