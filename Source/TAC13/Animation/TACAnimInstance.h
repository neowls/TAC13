// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "TACAnimInstance.generated.h"

UCLASS()
class TAC13_API UTACAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UTACAnimInstance();
	FORCEINLINE class ATACCharacterPlayer* GetCharacter() const { return Character; }
	
protected:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	void SetMovement();
	//void InterpAiming();

// Value Section
public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<class ATACCharacterPlayer> Character;
	
	UPROPERTY()
	float Direction;
	
	UPROPERTY()
	float GroundSpeed;
	
	UPROPERTY()
	float AimingAlpha;
	
	UPROPERTY()
	double AimPitch;

	UPROPERTY()
	FVector Velocity;

	UPROPERTY()
	uint8 bIsMoving : 1;

	UPROPERTY()
	uint8 bIsSprint : 1;

	UPROPERTY()
	uint8 bIsJumping : 1;
	
	UPROPERTY()
	uint8 bIsFalling : 1;

	UPROPERTY()
	uint8 bIsAiming : 1;

	UPROPERTY()
	uint8 bInterpAiming : 1;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UAnimMontage> FireMontage;
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UAnimMontage> AimFireMontage;
};
