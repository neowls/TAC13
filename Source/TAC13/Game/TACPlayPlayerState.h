
#pragma once

#include "CoreMinimal.h"
#include "TACPlayerState.h"
#include "GameData/TACStruct.h"
#include "TACPlayPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class TAC13_API ATACPlayPlayerState : public ATACPlayerState
{
	GENERATED_BODY()

public:
	ATACPlayPlayerState();

	FORCEINLINE FPlayerScore* GetPlayerScore() { return &PlayerScore; }

	UFUNCTION()
	void AddKillScore();

	UFUNCTION()
	void AddDeathScore();
	
	UFUNCTION()
	void OnRep_PlayerScore();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(ReplicatedUsing = OnRep_PlayerScore)
	FPlayerScore PlayerScore;
	
	UPROPERTY(Replicated)
	uint8 Ping;
};

