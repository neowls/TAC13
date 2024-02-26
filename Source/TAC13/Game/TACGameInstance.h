// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "TACGameInstance.generated.h"

USTRUCT(BlueprintType)
struct FServerInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	FString ServerName;

	UPROPERTY(BlueprintReadOnly)
	FString PlayerCountStr;

	UPROPERTY(BlueprintReadOnly)
	int32 CurrentPlayers = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 MaxPlayers = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 ServerArrayIndex;

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
	virtual void OnFindSessionsComplete(bool Succeeded);
	virtual void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	UFUNCTION(BlueprintCallable)
	void CreateServer(FString ServerName, FString HostName);

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
