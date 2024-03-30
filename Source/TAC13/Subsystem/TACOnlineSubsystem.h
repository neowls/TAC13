// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TACOnlineSubsystem.generated.h"


USTRUCT(BlueprintType)
struct FCreateSessionInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	FString RoomName;

	UPROPERTY(BlueprintReadWrite)
	int32 MaxPlayers = 0;

	UPROPERTY(BlueprintReadWrite)
	bool IsLan = false;
};

USTRUCT(BlueprintType)
struct FServerInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	FString SessionName;

	UPROPERTY(BlueprintReadOnly)
	FString PlayerCountStr;

	UPROPERTY(BlueprintReadOnly)
	bool IsLan = false;

	UPROPERTY(BlueprintReadOnly)
	int32 Ping = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 ServerArrayIndex = 0;
	
	int32 CurrentPlayers = 0;
	
	int32 MaxPlayers = 0;
	
	void SetPlayerCount()
	{
		PlayerCountStr = FString(FString::FromInt(CurrentPlayers) + "/" + FString::FromInt(MaxPlayers));
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnServerListDelegate, FServerInfo, ServerInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLobbySessionJoinDelegate, bool, Succeeded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSeachingServerDelegate, bool, bIsSearching);

UCLASS()
class TAC13_API UTACOnlineSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UTACOnlineSubsystem();

	UFUNCTION(BlueprintCallable)
	void CreateSession(FCreateSessionInfo InCreateSessionInfo);

	UFUNCTION(BlueprintCallable)
	void JoinIndexSession(int32 ArrayIndex);

	UFUNCTION(BlueprintCallable)
	void FindSession();

	UFUNCTION(BlueprintCallable)
	void DestroySession();

	FORCEINLINE FName GetSessionName() const { return SessionNameKey; }
	FString GetSessionName(const FOnlineSessionSearchResult& SearchResult) const;
	
protected:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	virtual void OnCreateSessionComplete(FName SessionName, bool Succeeded);
	virtual void OnDestroySessionComplete(FName SessionName, bool Succeeded);
	virtual void OnFindSessionsComplete(bool Succeeded);
	virtual void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	virtual void OnSessionParticipantChanged(FName SessionName, const FUniqueNetId& PlayerId, bool IsJoin);

private:
	class IOnlineSubsystem* OnlineSubsystem;
	
	IOnlineSessionPtr SessionInterface;

	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	TSharedPtr<FOnlineSessionSettings>	SessionSettings;

	FCreateSessionInfo LastCreateSessionInfo;

	UPROPERTY(BlueprintAssignable)
	FOnServerListDelegate OnServerList;

	UPROPERTY(BlueprintAssignable)
	FOnSeachingServerDelegate OnSearchingServer;

	UPROPERTY(BlueprintAssignable)
	FOnLobbySessionJoinDelegate OnLobbySessionJoined;
	
	bool bCreateSessionOnDestroy { false };
	const FName SessionNameKey { "SessionNameKey" };
	
	FString LobbyLevelPath { "/Game/_TAC/Menu/Lobby"};
	FString MenuLevelPath { "/Game/_TAC/Menu/Menu"};
};
