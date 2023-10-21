// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TACCharacterBase.generated.h"

UCLASS()
class TAC13_API ATACCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATACCharacterBase(const FObjectInitializer& ObjectInitializer);

	virtual void PostInitializeComponents() override;
	
protected:
	virtual void SetCharacterControlData(const class UTACControlData* CharacterControlData);

	UPROPERTY(EditAnywhere, Category = CharacterControl, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTACControlData> CurrentControlData;
};
