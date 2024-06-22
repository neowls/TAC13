//

#pragma once

#include "CoreMinimal.h"
#include "Player/TACPlayerController.h"
#include "UI/TACLobbyWidget.h"
#include "Interface/TACPlayerStateRequestInterface.h"
#include "TACLobbyPlayerController.generated.h"

class ATACLobbyGameState;
class ATACLobbyPlayerState;

UCLASS()
class TAC13_API ATACLobbyPlayerController : public ATACPlayerController, public ITACPlayerStateRequestInterface
{
	GENERATED_BODY()
	
	ATACLobbyPlayerController(const FObjectInitializer& ObjectInitializer);
	
public:
	virtual void PostInitializeComponents() override;
	
	virtual void BeforeSetUI() override;
	
	virtual void SetUIWidget() override;

	virtual void ReceivedPlayer() override;
	
	virtual void RequestUpdatePlayerInfo() override;
	
	UFUNCTION(Server,Reliable)
	void Server_RequestUpdatePlayerList();

	UFUNCTION(Server, Reliable)
	void Server_ChangePlayerReadyState();

protected:
	UPROPERTY()
	TObjectPtr<UTACLobbyWidget> LobbyWidget;

	UPROPERTY()
	TObjectPtr<ATACLobbyGameState> LobbyGameState;
};
