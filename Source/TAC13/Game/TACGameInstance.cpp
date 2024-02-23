// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/TACGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

UTACGameInstance::UTACGameInstance()
{
	
}

void UTACGameInstance::Init()
{
	Super::Init();
	if(IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get()) // 정적 선언된 온라인 서브시스템 참조
	{
		SessionInterface = Subsystem->GetSessionInterface();
		if(SessionInterface.IsValid())
		{
			//Bind Delegates
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UTACGameInstance::OnCreateSessionComplete); // 세션 생성 델리게이트
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UTACGameInstance::OnFindSessionsComplete); // 세션 검색 델리게이트
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UTACGameInstance::OnJoinSessionComplete); // 세션 참가 델리게이트
		}
	}
	
}

void UTACGameInstance::OnCreateSessionComplete(FName ServerName, bool Succeeded) // 세션을 성공적으로 생성한다면 서버상에서 특정맵으로 이동
{
	UE_LOG(LogTemp, Warning, TEXT("OnCreateSessionComplete, Succeeded : %d") , Succeeded);
	if(Succeeded)
	{
		GetWorld()->ServerTravel("/Game/_TAC/Maps/DevLevel?listen");
	}
}

void UTACGameInstance::OnFindSessionsComplete(bool Succeeded) // 세션을 성공적으로 탐색했다면 해당 세션들의 정보를 가져와 이를 Broadcast 함
{
	UE_LOG(LogTemp, Warning, TEXT("OnFindSessionsComplete, Succeeded : %d") , Succeeded);
	if(Succeeded)
	{
		TArray<FOnlineSessionSearchResult> SearchResults = SessionSearch->SearchResults;

		for(FOnlineSessionSearchResult Result : SearchResults)
		{
			if(!Result.IsValid()) continue;
			FServerInfo Info;
			FString ServerName = "Empty Server Name";
			FString HostName = "Empty Host Name";

			Result.Session.SessionSettings.Get(FName("SERVER_NAME_KEY"), ServerName);
			Result.Session.SessionSettings.Get(FName("SERVER_HOSTNAME_KEY"), HostName);
			
			Info.ServerName = ServerName;
			Info.MaxPlayers = Result.Session.SessionSettings.NumPublicConnections;
			Info.CurrentPlayers = Info.MaxPlayers - Result.Session.NumOpenPublicConnections;
			Info.SetPlayerCount();
			
			ServerListDelegate.Broadcast(Info);
		}
		
		UE_LOG(LogTemp, Warning, TEXT("Search Results, Server Count : %d") , SearchResults.Num());
		
		if(SearchResults.Num())
		{
			/*UE_LOG(LogTemp, Warning, TEXT("Joining Server"));
			SessionInterface->JoinSession(0, FName("TAC"), SearchResults[0]);*/
		}

	}
}

void UTACGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result) // 타겟 세션에 성공적으로 참여했을 경우 세션 맵으로 클라이언트 맵 이동
{
	UE_LOG(LogTemp, Warning, TEXT("OnJoinSessionComplete, SessionName : %s"), *SessionName.ToString());
	if(APlayerController* PC = GetFirstLocalPlayerController())
	{
		FString JoinAddress = "";
		SessionInterface->GetResolvedConnectString(SessionName, JoinAddress);
		if(JoinAddress != "")
			PC->ClientTravel(JoinAddress, TRAVEL_Absolute);
	}
}


void UTACGameInstance::CreateServer(FString ServerName, FString HostName) // 세션 생성
{
	UE_LOG(LogTemp, Warning, TEXT("Created Server"));
	
	FOnlineSessionSettings SessionSettings;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bIsDedicated = false;
	if(IOnlineSubsystem::Get()->GetSubsystemName() != "NULL")
	{
		SessionSettings.bIsLANMatch = false;
		SessionSettings.bUseLobbiesIfAvailable = false;
	}
	else
	{
		SessionSettings.bIsLANMatch = true;
		SessionSettings.bUseLobbiesIfAvailable = true;
	}
	
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bUsesPresence = true;
	
	SessionSettings.NumPublicConnections = 8;
	
	

	SessionSettings.Set(FName("SERVER_NAME_KEY"), ServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	SessionSettings.Set(FName("SERVER_HOSTNAME_KEY"), HostName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	
	SessionInterface->CreateSession(0, FName("TAC"), SessionSettings);
	
}

void UTACGameInstance::JoinServer()
{
	UE_LOG(LogTemp, Warning, TEXT("Join Server"));
	//SessionInterface->JoinSession(0, FName("TAC"), SearchResults[0]);
}

void UTACGameInstance::FindServer() // 세션 탐색
{
	UE_LOG(LogTemp, Warning, TEXT("Find Server"));
	
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
