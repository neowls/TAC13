// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/TACAnimInstance.h"
#include "TACArmAnimInstance.generated.h"


UCLASS()
class TAC13_API UTACArmAnimInstance : public UTACAnimInstance
{
	GENERATED_BODY()

protected:
	virtual void NativeBeginPlay() override;
	
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	// Value Section
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<class ATACCharacterPlayer> OwnerPlayer;

public:
	UTACArmAnimInstance();

	void SetNewSight();
	
	void SetSightTransform();
	
	void SetRelativeHandTransform();
	
	void SetLeftHandTransform();

	void InterpAiming();

protected:
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	FTransform RelativeHandTransform;
	
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	FTransform SightTransform;
	
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	FTransform LeftHandTransform;

	uint8 bInterpAiming : 1;
	
};
