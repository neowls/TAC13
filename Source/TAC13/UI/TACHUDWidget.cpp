// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/TACHUDWidget.h"

#include "Interface/TACCharacterHUDInterface.h"
#include "TACHPWidget.h"
#include "TACKillLogWidget.h"
#include "TACScoreBoardWidget.h"
#include "TACWeaponWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/VerticalBox.h"

UTACHUDWidget::UTACHUDWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UTACUserWidget> KillLogWidgetRef(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/_TAC/UI/WBP_KillLog.WBP_KillLog_C'"));
	if(KillLogWidgetRef.Class)
	{
		KillLogClass = KillLogWidgetRef.Class;
	}
}

void UTACHUDWidget::UpdateCurrentAmmo(uint8 NewCurrentAmmo)
{
	WeaponStat->UpdateCurrentAmmo(NewCurrentAmmo);
}

void UTACHUDWidget::UpdateOwnAmmo(uint8 NewOwnAmmo)
{
	WeaponStat->UpdateOwnAmmo(NewOwnAmmo);
}

void UTACHUDWidget::UpdateWeaponName(FString NewWeaponName)
{
	WeaponStat->UpdateWeaponName(NewWeaponName);
}

void UTACHUDWidget::UpdateHPBar(uint8 NewCurrentHP)
{
	HPBar->UpdateHP(NewCurrentHP);
}

void UTACHUDWidget::UpdateKillLog(FString InAttacker, FString InWeaponName, FString InVictim)
{
	UTACKillLogWidget* InKillLog = Cast<UTACKillLogWidget>(CreateWidget(this, KillLogClass));
	KillLogBox->AddChildToVerticalBox(InKillLog);
	InKillLog->UpdateKillLogText(InAttacker, InWeaponName, InVictim);
}

void UTACHUDWidget::UpdateScoreBoard()
{
	if(ScoreBoard.IsValid())
	ScoreBoard->UpdateScoreBoard();
}

void UTACHUDWidget::SetScoreBoardPlayer(const TArray<ATACPlayerState*>& InArray)
{
	if(ScoreBoard.IsValid())
	ScoreBoard->SetPlayerList(InArray);
}

void UTACHUDWidget::ScoreBoardOnOff(uint8 bIsScoreBoardOn)
{
	if(!ScoreBoard.IsValid()) return;
	if(bIsScoreBoardOn) ScoreBoard->SetVisibility(ESlateVisibility::Visible);
	else ScoreBoard->SetVisibility(ESlateVisibility::Hidden);
}


void UTACHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	ITACCharacterHUDInterface* HUDPawn = Cast<ITACCharacterHUDInterface>(GetOwningPlayerPawn());
	if(HUDPawn)
	{
		HUDPawn->SetupHUDWidget(this);
	}
}
