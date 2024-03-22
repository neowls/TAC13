// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/TACGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "TAC13.h"
#include "GameFramework/PlayerState.h"

UTACGameInstance::UTACGameInstance()
{
	CurrentSessionName = FName("TAC13");
}

void UTACGameInstance::Init()
{
	Super::Init();
	IOnlineIdentityPtr IdentityPtr = IOnlineSubsystem::Get()->GetIdentityInterface();
	if(const IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get()) // 정적 선언된 온라인 서브시스템 참조
	{
		
		SessionInterface = Subsystem->GetSessionInterface();
		if(SessionInterface.IsValid())
		{
			//Bind Delegates
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UTACGameInstance::OnCreateSessionComplete);	// 세션 생성
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UTACGameInstance::OnFindSessionsComplete);		// 세션 검색
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UTACGameInstance::OnJoinSessionComplete);		// 세션 참가
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UTACGameInstance::OnDestroySessionComplete);	// 세션 나가기
			SessionInterface->OnSessionParticipantsChangeDelegates.AddUObject(this, &UTACGameInstance::OnSessionParticipantChanged);	//	세션 참가자 변동
		}
	}
	
}

void UTACGameInstance::OnCreateSessionComplete(FName SessionName, bool Succeeded)	// 세션을 성공적으로 생성한다면 서버상에서 특정맵으로 이동
{
	UE_LOG(LogTemp, Warning, TEXT("Session Create %s") , Succeeded ? TEXT("Succeed") : TEXT("Failed") );
	/*if(Succeeded)
	{
		GetWorld()->ServerTravel("/Game/_TAC/Maps/DevLevel?listen");
	}*/
	
}

void UTACGameInstance::OnDestroySessionComplete(FName SessionName, bool Succeeded)
{
	UE_LOG(LogTemp, Warning, TEXT("OnDestroySessionComplete, Succeeded : %d") , Succeeded);
}

void UTACGameInstance::OnFindSessionsComplete(bool Succeeded)	// 세션을 성공적으로 탐색했다면 해당 세션들의 정보를 가져와 이를 Broadcast 함
{
	OnSearchingServer.Broadcast(false);
	UE_LOG(LogTemp, Warning, TEXT("OnFindSessionsComplete, Succeeded : %d") , Succeeded);
	if(Succeeded)
	{
		TArray<FOnlineSessionSearchResult> SearchResults = SessionSearch->SearchResults;
		int32 ArrayIndex = -1;	
		for(FOnlineSessionSearchResult Result : SearchResults)
		{
			++ArrayIndex;
			if(!Result.IsValid()) continue;
			FServerInfo Info;
			
			FString ServerName = "Empty Server Name";
			FString HostName = "Empty Host Name";

			Result.Session.SessionSettings.Get(FName("SESSION_ROOM_NAME_KEY"), ServerName);
			Result.Session.SessionSettings.Get(FName("SESSION_HOSTNAME_KEY"), HostName);
			
			Info.SessionName = ServerName;
			Info.MaxPlayers = Result.Session.SessionSettings.NumPublicConnections;
			Info.CurrentPlayers = Info.MaxPlayers - Result.Session.NumOpenPublicConnections;

			Info.SetPlayerCount();
			
			Info.IsLan = Result.Session.SessionSettings.bIsLANMatch;
			Info.Ping = Result.PingInMs;
			Info.ServerArrayIndex = ArrayIndex;
			
			OnServerList.Broadcast(Info);
		}
		
		UE_LOG(LogTemp, Warning, TEXT("Search Results, Server Count : %d") , SearchResults.Num());
	}
}

void UTACGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)	// 타겟 세션에 성공적으로 참여했을 경우 세션 맵으로 클라이언트 맵 이동
{
	UE_LOG(LogTemp, Warning, TEXT("OnJoinSessionComplete, SessionName : %s"), *SessionName.ToString());
	/*if(APlayerController* PC = GetFirstLocalPlayerController())
	{
		FString JoinAddress = "";
		SessionInterface->GetResolvedConnectString(SessionName, JoinAddress);
		if(JoinAddress != "")
			PC->ClientTravel(JoinAddress, TRAVEL_Absolute);
	}*/
	if(Result == EOnJoinSessionCompleteResult::Success)
	{
		
	}
}

void UTACGameInstance::OnSessionParticipantChanged(FName SessionName, const FUniqueNetId& PlayerId, bool IsJoin)
{
	UE_LOG(LogTemp, Warning, TEXT("Player Joined"));
}


void UTACGameInstance::CreateSession(FCreateSessionInfo InCreateSessionInfo)	// 세션 생성
{
	LOG_SCREEN(0, TEXT("Created Server"));
	UE_LOG(LogTemp, Warning, TEXT("Created Server"));
	
	FOnlineSessionSettings SessionSettings;
	SessionSettings.bAllowJoinInProgress = true;		// 진행중인 세션에 참가 가능한지?
	SessionSettings.bIsDedicated = false;				// 데디 케이트 서버인가?
	SessionSettings.bIsLANMatch = InCreateSessionInfo.IsLan;
	SessionSettings.bUseLobbiesIfAvailable = true;
	
	SessionSettings.bShouldAdvertise = true;			// 온라인 매치메이킹에 공개되는지?
	SessionSettings.bUsesPresence = true;				// 참여자가 서로의 온라인 상태를 확인할 수 있는지?
	SessionSettings.NumPublicConnections = InCreateSessionInfo.MaxPlayers;
	
	

	SessionSettings.Set(FName("SESSION_ROOM_NAME_KEY"), InCreateSessionInfo.SessionRoomName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	SessionSettings.Set(FName("SESSION_HOSTNAME_KEY"), GetFirstLocalPlayerController()->PlayerState->GetPlayerName(), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	
	SessionInterface->CreateSession(0, CurrentSessionName, SessionSettings);
	
}

void UTACGameInstance::JoinServer(int32 ArrayIndex)
{
	LOG_SCREEN(0, TEXT("Join Server"));
	UE_LOG(LogTACNetwork, Warning, TEXT("Join Server"));
	
	FOnlineSessionSearchResult Result = SessionSearch->SearchResults[ArrayIndex];
	if(Result.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("JOINING AT INDEX : %d"), ArrayIndex);
		SessionInterface->JoinSession(0, CurrentSessionName, Result);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("FAILED TO JOIN SERVER AT INDEX : %d"), ArrayIndex);
	}
}

void UTACGameInstance::FindServer() // 세션 탐색
{
	LOG_SCREEN(0, TEXT("Find Server"));
	UE_LOG(LogTACNetwork, Warning, TEXT("Find Server"));
	OnSearchingServer.Broadcast(true);
	
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	
	if(IOnlineSubsystem::Get()->GetSubsystemName() != "NULL")
	{
		SessionSearch->bIsLanQuery = false;		
	}
	else
	{
		SessionSearch->bIsLanQuery = true; // Is Lan
	}
	
	SessionSearch->MaxSearchResults = 15000;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}
