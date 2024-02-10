// Fill out your copyright notice in the Description page of Project Settings.


#include "TACGameState.h"
#include "Net/UnrealNetwork.h"
#include "Player/TACPlayerController.h"
#include "UI/TACHUDWidget.h"


ATACGameState::ATACGameState()
{
	
}

void ATACGameState::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATACGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATACGameState, KillLogList);
}

void ATACGameState::SetPlayerList()
{
	for(auto it : PlayerArray)
	{
		
	}
}

void ATACGameState::OnRep_KillLogUpdated()
{
	ATACPlayerController* PlayerController = Cast<ATACPlayerController>(GetWorld()->GetFirstPlayerController());
	if (PlayerController)
	{
		// 플레이어 컨트롤러를 통해 킬로그 위젯 인스턴스에 접근
		if (PlayerController->GetTACHUDWidget())
		{
			// 킬로그 위젯에 모든 킬로그 엔트리를 추가 또는 업데이트
			PlayerController->GetTACHUDWidget()->UpdateKillLog(KillLogList.Last().KillerName, "Weapon", KillLogList.Last().VictimName);
		}
	}
}

void ATACGameState::AddKillLogEntry(const FString& KillerName, const FString& VictimName)
{
	if (GetLocalRole() == ROLE_Authority) // 서버에서만 실행
	{
		FKillLogEntry NewEntry;
		NewEntry.KillerName = KillerName;
		NewEntry.VictimName = VictimName;

		KillLogList.Add(NewEntry);
		
		OnRep_KillLogUpdated();
	}
}
