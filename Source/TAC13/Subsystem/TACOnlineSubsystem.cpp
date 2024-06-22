// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/TACOnlineSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "TAC13.h"
#include "GameFramework/PlayerState.h"

UTACOnlineSubsystem::UTACOnlineSubsystem()
{
	OnlineSubsystem = nullptr;
}

FString UTACOnlineSubsystem::GetSessionName(const FOnlineSessionSearchResult& SearchResult) const
{
	FString outVal = {"Name None"};
	SearchResult.Session.SessionSettings.Get(NAME_GameSession, outVal);
	return outVal;
}

void UTACOnlineSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	OnlineSubsystem = IOnlineSubsystem::Get(); // 정적 선언된 온라인 서브시스템 참조
	SessionInterface = OnlineSubsystem->GetSessionInterface(); // 세션 인터페이스 캐싱
	if(SessionInterface.IsValid())
	{
		UE_LOG(LogTACNetwork, Warning, TEXT("Session Interface Found"));
		//Bind Delegates
		SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &ThisClass::OnCreateSessionComplete);			// 세션 생성
		SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &ThisClass::OnFindSessionsComplete);				// 세션 검색
		SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &ThisClass::OnJoinSessionComplete);				// 세션 참가
		SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &ThisClass::OnDestroySessionComplete);			// 세션 나가기
		SessionInterface->OnSessionParticipantsChangeDelegates.AddUObject(this, &ThisClass::OnSessionParticipantChanged);	// 세션 참가자 변동
	}
	LOG_SCREEN(0, TEXT("Subsystem Name : %s"), *IOnlineSubsystem::Get()->GetSubsystemName().ToString());
	
	UE_LOG(LogTACNetwork, Warning,  TEXT("SessionState Name : %d"), SessionInterface->GetSessionState(NAME_GameSession));
	LOG_SCREEN(1, TEXT("SessionState Name : %d"), SessionInterface->GetSessionState(NAME_GameSession));
}

void UTACOnlineSubsystem::OnSessionParticipantChanged(FName SessionName, const FUniqueNetId& PlayerId, bool IsJoin)
{
	UE_LOG(LogTACNetwork, Warning, TEXT("Player Id : %s"), *PlayerId.ToString());
}

void UTACOnlineSubsystem::StartGameSession()
{
	GetWorld()->ServerTravel(GameLevelPath + "?listen", false);
	SessionInterface->StartSession(NAME_GameSession);
}

void UTACOnlineSubsystem::CreateSession(FCreateSessionInfo InCreateSessionInfo)	// 세션 생성
{
	UE_LOG(LogTACNetwork, Warning, TEXT("Create Session"));
	LOG_SCREEN(2, TEXT("Create Session"));
	
	UE_LOG(LogTACNetwork, Warning,  TEXT("SessionState Name : %d"), SessionInterface->GetSessionState(NAME_GameSession));
	LOG_SCREEN(3, TEXT("SessionState Name : %d"), SessionInterface->GetSessionState(NAME_GameSession));


	if(!SessionInterface.IsValid()) return;
	if (SessionInterface->GetSessionState(NAME_GameSession) != EOnlineSessionState::NoSession)	// 동일한 세션이 있다면 해당 세션을 삭제한다.
	{
		bCreateSessionOnDestroy = true;
		LastCreateSessionInfo = InCreateSessionInfo;
		DestroySession();
	}
	
	SessionSettings = MakeShareable(new FOnlineSessionSettings);	// 생성할 세션 세팅 포인터 준비
	
	SessionSettings->bAllowJoinInProgress	= true;		// 진행중인 세션에 참가 가능한지?
	SessionSettings->bAllowJoinViaPresence	= true;		// 플레이어를 통한 세션 참가가 가능한지? (ex. 특정 플레이어가 게임중이면 그 해당 세션에 참가하는 방법)
	SessionSettings->bUseLobbiesIfAvailable = true;		// 로비 기능을 사용할 것인가?
	SessionSettings->bShouldAdvertise		= true;		// 온라인 매치메이킹에 공개되는지?
	SessionSettings->bUsesPresence			= true;		// 참여자가 서로의 온라인 상태를 확인할 수 있는지?
	SessionSettings->bIsDedicated			= false;
	SessionSettings->bIsLANMatch			= InCreateSessionInfo.IsLan && IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";	// Lan 매치인가?
	UE_LOG(LogTemp, Warning, TEXT("Subsystem Name : %s"), *IOnlineSubsystem::Get()->GetSubsystemName().ToString());
	SessionSettings->NumPublicConnections	= InCreateSessionInfo.MaxPlayers;	//최대 가용 인원
	SessionSettings->BuildUniqueId			= 4;		// 검색 중에 다른 빌드가 서로 보이지 않도록 하는데 사용된다.
	
	// 클라이언트에게 임의의 데이터를 생성하고 전달할 수 있게 설정한다.
	SessionSettings->Set(FName("SESSION_ROOM_NAME_KEY"), InCreateSessionInfo.RoomName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	SessionSettings->Set(FName("SESSION_HOSTNAME_KEY"), GetWorld()->GetFirstPlayerController()->PlayerState->GetPlayerName(), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	
	if (!SessionInterface->CreateSession(0, NAME_GameSession, *SessionSettings))
	{
		UE_LOG(LogTACNetwork, Warning, TEXT("Failed to Create Session"));
		LOG_SCREEN(5, TEXT("Failed to Create Session"));
	}
}

void UTACOnlineSubsystem::FindSession() // 세션 탐색
{
	LOG_SCREEN(2, TEXT("Find Session"));
	UE_LOG(LogTACNetwork, Warning, TEXT("Find Session"));
	
	OnSearchingServer.Broadcast(true);
	SessionSearch = MakeShareable(new FOnlineSessionSearch);
	
	LOG_SCREEN(3, TEXT("Subsystem Name : %s"), *IOnlineSubsystem::Get()->GetSubsystemName().ToString());
	if(IOnlineSubsystem::Get()->GetSubsystemName() != "NULL")
	{
		SessionSearch->bIsLanQuery = false;		
	}
	else
	{
		SessionSearch->bIsLanQuery = true; // Is Lan
	}
	SessionSearch->MaxSearchResults = 100000;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	
	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

void UTACOnlineSubsystem::JoinIndexSession(int32 ArrayIndex)
 {
 	LOG_SCREEN(0, TEXT("Join Server"));
 	UE_LOG(LogTACNetwork, Warning, TEXT("Join Server"));
 	
 	const FOnlineSessionSearchResult Result = SessionSearch->SearchResults[ArrayIndex];
 	if(Result.IsValid())
 	{
 		UE_LOG(LogTACNetwork, Warning, TEXT("Success Index : %d"), ArrayIndex);
 		SessionInterface->JoinSession(0, NAME_GameSession, Result);
 	}
 	else
 	{
 		UE_LOG(LogTACNetwork, Warning, TEXT("Failed Index : %d"), ArrayIndex);
 	}
 }

void UTACOnlineSubsystem::DestroySession()
{
	if(SessionInterface->GetSessionState(NAME_GameSession) == EOnlineSessionState::NoSession) return;
	if(SessionInterface->DestroySession(NAME_GameSession))
	{
		UE_LOG(LogTACNetwork, Warning, TEXT("Destroy Session Success"));
		LOG_SCREEN(7, TEXT("Destroy Session Success"));
	}
	else
	{
		UE_LOG(LogTACNetwork, Warning, TEXT("Destroy Session Failed"));
		LOG_SCREEN(8, TEXT("Destroy Session Failed"));
	}
}

void UTACOnlineSubsystem::OnCreateSessionComplete(FName SessionName, bool Succeeded)	// 세션을 성공적으로 생성한다면 서버상에서 특정맵으로 이동
{
	UE_LOG(LogTACNetwork, Warning, TEXT("Create Session %s") , Succeeded ? TEXT("Succeed") : TEXT("Failed"));
	LOG_SCREEN(6, TEXT("Create Session Complete"));
	if(Succeeded)
	{
		OnLobbySessionJoined.Broadcast(GetWorld()->ServerTravel(LobbyLevelPath + "?listen", true));
	}
}

void UTACOnlineSubsystem::OnFindSessionsComplete(bool Succeeded)	// 세션을 성공적으로 탐색했다면 해당 세션들의 정보를 가져와 이를 Broadcast 함
{
	OnSearchingServer.Broadcast(false);
	
	UE_LOG(LogTACNetwork, Warning, TEXT("Find Session %s") , Succeeded ? TEXT("Succeed") : TEXT("Failed"));
	LOG_SCREEN(0, TEXT("Find Session Complete"));
	
	if(Succeeded)
	{
		TArray<FOnlineSessionSearchResult>& SearchResults = SessionSearch->SearchResults;
		int32 ArrayIndex = -1;	
		for(FOnlineSessionSearchResult Result : SearchResults)
		{
			++ArrayIndex;
			int32 LOGIndex = 0;
			if(!Result.IsValid()) continue;
			FServerInfo Info;
			
			FString SessionName = "Empty Session Name";
			FString HostName = "Empty Host Name";

			Result.Session.SessionSettings.Get(FName("SESSION_ROOM_NAME_KEY"), SessionName);
			Result.Session.SessionSettings.Get(FName("SESSION_HOSTNAME_KEY"), HostName);
			
			Info.SessionName = SessionName;
			Info.MaxPlayers = Result.Session.SessionSettings.NumPublicConnections;
			Info.CurrentPlayers = Info.MaxPlayers - Result.Session.NumOpenPublicConnections;

			Info.SetPlayerCount();
			
			Info.IsLan = Result.Session.SessionSettings.bIsLANMatch;
			Info.Ping = Result.PingInMs;
			Info.ServerArrayIndex = ArrayIndex;

			LOG_SCREEN(++LOGIndex, TEXT("Session Name : %s"), *SessionName);
			LOG_SCREEN(++LOGIndex, TEXT("Available Player Num : %d"), Result.Session.NumOpenPublicConnections);
			LOG_SCREEN(++LOGIndex, TEXT("Host Name : %s"), *HostName);
			
			UE_LOG(LogTACNetwork, Warning, TEXT("Found Session ID : %s"), *Result.GetSessionIdStr());
			
			OnServerList.Broadcast(Info);
		}
		
		UE_LOG(LogTACNetwork, Warning, TEXT("Session Count : %d") , SearchResults.Num());
	}
}

void UTACOnlineSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)	// 타겟 세션에 성공적으로 참여했을 경우 세션 맵으로 클라이언트 맵 이동
{
	switch (Result)
	{
	case EOnJoinSessionCompleteResult::Success :
		LOG_SCREEN(0, TEXT("SUCCESS"));
		UE_LOG(LogTACNetwork, Warning, TEXT("SUCCESS"));
		break;

	case EOnJoinSessionCompleteResult::AlreadyInSession :
		LOG_SCREEN(0, TEXT("ALREADY IN SESSION"));
		UE_LOG(LogTACNetwork, Warning, TEXT("ALREADY IN SESSION"));
		break;

	case EOnJoinSessionCompleteResult::UnknownError :
		LOG_SCREEN(0, TEXT("UNKNOWN ERROR"));
		UE_LOG(LogTACNetwork, Warning, TEXT("UNKNOWN ERROR"));
		break;

	default:
		LOG_SCREEN(0, TEXT("DEFAULT"));
		UE_LOG(LogTACNetwork, Warning, TEXT("DEFAULT"));
		break;
	}
	
	if(Result == EOnJoinSessionCompleteResult::Success)
	{
		FString TravelAddress = "";
		SessionInterface->GetResolvedConnectString(SessionName, TravelAddress);
		UE_LOG(LogTACNetwork, Warning, TEXT("Travel Address : %s"), *TravelAddress);
		GetGameInstance()->GetFirstLocalPlayerController()->ClientTravel(TravelAddress, TRAVEL_Absolute);
		OnLobbySessionJoined.Broadcast(true);
	}

}

void UTACOnlineSubsystem::OnDestroySessionComplete(FName SessionName, bool Succeeded)
{
	UE_LOG(LogTACNetwork, Warning, TEXT("Session Name : %s /  %s"), *SessionName.ToString(), Succeeded ? TEXT("Succeed") : TEXT("Failed"));
	if(Succeeded)
	{
		if (!bCreateSessionOnDestroy)
		{
			GetGameInstance()->GetFirstLocalPlayerController()->ClientTravel(MenuLevelPath, TRAVEL_Absolute);
		}
		else
		{
			bCreateSessionOnDestroy = false;
		}
		UE_LOG(LogTACNetwork, Warning, TEXT("Session Destroyed"));
	}
	else
	{
		UE_LOG(LogTACNetwork, Warning, TEXT("Can't Destroy Session"));
	}
}

