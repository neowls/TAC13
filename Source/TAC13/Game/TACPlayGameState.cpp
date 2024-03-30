// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/TACPlayGameState.h"
#include "Net/UnrealNetwork.h"
#include "Player/TACPlayPlayerController.h"
#include "UI/TACHUDWidget.h"

ATACPlayGameState::ATACPlayGameState()
{
}


void ATACPlayGameState::BeginPlay()
{
	Super::BeginPlay();
	FirstPlayerController = Cast<ATACPlayPlayerController>(GetWorld()->GetFirstPlayerController());
}

void ATACPlayGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATACPlayGameState, KillLogList);
	DOREPLIFETIME(ATACPlayGameState, TACPlayerList);
}

void ATACPlayGameState::OnRep_KillLogUpdated()
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

void ATACPlayGameState::OnRep_PlayerList()
{
	if (FirstPlayerController && FirstPlayerController->GetTACHUDWidget() != nullptr)
	{
		FirstPlayerController->GetTACHUDWidget()->SetScoreBoardPlayer(TACPlayerList);
		FirstPlayerController->GetTACHUDWidget()->UpdateScoreBoard();
	}
	
}

void ATACPlayGameState::AddKillLogEntry(const FString& KillerName, const FString& WeaponName, const FString& VictimName)
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

