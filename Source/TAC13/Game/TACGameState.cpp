// Fill out your copyright notice in the Description page of Project Settings.


#include "TACGameState.h"
#include "TACPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Player/TACPlayerController.h"
#include "UI/TACHUDWidget.h"


ATACGameState::ATACGameState()
{
	
}

void ATACGameState::BeginPlay()
{
	Super::BeginPlay();
	FirstPlayerController = Cast<ATACPlayerController>(GetWorld()->GetFirstPlayerController());
}

void ATACGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATACGameState, KillLogList);
	DOREPLIFETIME(ATACGameState, TACPlayerList);
}

void ATACGameState::OnRep_KillLogUpdated()
{
	if (FirstPlayerController)
	{
		// 플레이어 컨트롤러를 통해 킬로그 위젯 인스턴스에 접근
		if (FirstPlayerController->GetTACHUDWidget())
		{
			// 킬로그 위젯에 모든 킬로그 엔트리를 추가 또는 업데이트
			FirstPlayerController->GetTACHUDWidget()->UpdateKillLog(KillLogList.Last().KillerName, KillLogList.Last().WeaponName, KillLogList.Last().VictimName);
		}
	}
}

void ATACGameState::OnRep_PlayerList()
{
	if (FirstPlayerController && FirstPlayerController->GetTACHUDWidget() != nullptr)
	{
		FirstPlayerController->GetTACHUDWidget()->SetScoreBoardPlayer(TACPlayerList);
		FirstPlayerController->GetTACHUDWidget()->UpdateScoreBoard();
	}
	
}

void ATACGameState::AddKillLogEntry(const FString& KillerName, const FString& WeaponName, const FString& VictimName)
{
	if (GetLocalRole() == ROLE_Authority) // 서버에서만 실행
	{
		FKillLogEntry NewEntry;
		NewEntry.KillerName = KillerName;
		NewEntry.WeaponName = WeaponName;
		NewEntry.VictimName = VictimName;

		KillLogList.Add(NewEntry);
		
		OnRep_KillLogUpdated();
	}
}
