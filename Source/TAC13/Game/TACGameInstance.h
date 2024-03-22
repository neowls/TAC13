// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "TACGameInstance.generated.h"

USTRUCT(BlueprintType)
struct FCreateSessionInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	FString SessionRoomName;

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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSeachingServerDelegate, bool, bIsSearching);

/**
 * 
 */
UCLASS()
class TAC13_API UTACGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UTACGameInstance();

protected:
	virtual void Init() override;

	virtual void OnCreateSessionComplete(FName SessionName, bool Succeeded);
	virtual void OnDestroySessionComplete(FName SessionName, bool Succeeded);
	virtual void OnFindSessionsComplete(bool Succeeded);
	virtual void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	virtual void OnSessionParticipantChanged(FName SessionName, const FUniqueNetId& PlayerId, bool IsJoin);

	

	UFUNCTION(BlueprintCallable)
	void CreateSession(FCreateSessionInfo InCreateSessionInfo);

	UFUNCTION(BlueprintCallable)
	void JoinServer(int32 ArrayIndex);

	UFUNCTION(BlueprintCallable)
	void FindServer();

	

	FName CurrentSessionName;

	IOnlineSessionPtr SessionInterface;

	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	UPROPERTY(BlueprintAssignable)
	FOnServerListDelegate OnServerList;

	UPROPERTY(BlueprintAssignable)
	FOnSeachingServerDelegate OnSearchingServer;
};
