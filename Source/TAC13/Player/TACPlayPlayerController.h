//

#pragma once

#include "CoreMinimal.h"
#include "Player/TACPlayerController.h"
#include "TACPlayPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class TAC13_API ATACPlayPlayerController : public ATACPlayerController
{
	GENERATED_BODY()

public:

	ATACPlayPlayerController();

	FORCEINLINE class UTACHUDWidget* GetTACHUDWidget() const { return TACHUDWidget; }

protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void SetUIWidget() override;
	virtual void SetHUDWidget();
	virtual void OnPossess(APawn* InPawn) override;
	virtual void PostNetInit() override;
	virtual void OnRep_Owner() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	TSubclassOf<class UTACHUDWidget> TACHUDWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = HUD)
	TObjectPtr<class UTACHUDWidget> TACHUDWidget;
};
