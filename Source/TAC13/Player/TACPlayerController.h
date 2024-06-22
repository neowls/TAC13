//

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TACPlayerController.generated.h"

class UTACUserWidget;

UCLASS()
class TAC13_API ATACPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ATACPlayerController();

protected:
	virtual void BeginPlay() override;

	virtual void BeforeSetUI();

	virtual void PostInitializeComponents() override;

	virtual void SetUIWidget();

	virtual void OnPossess(APawn* InPawn) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	TSubclassOf<class UTACUserWidget> UIWidgetClass;

	TObjectPtr<class UTACUserWidget> UIWidget;

};
