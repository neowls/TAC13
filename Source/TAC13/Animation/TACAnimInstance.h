// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Character/TACCharacterBase.h"
#include "TACAnimInstance.generated.h"


UENUM(BlueprintType)
enum class EAnim_RootYawOffsetMode : uint8
{
	BlendOut,
	Hold,
	Accumulate
};


UCLASS()
class TAC13_API UTACAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UTACAnimInstance();
	
	FORCEINLINE class ATACCharacterBase* GetOwner() const { return Owner; }

	UFUNCTION(BlueprintPure)
	UCharacterMovementComponent* GetCharacterMovementComponent() const { return Owner->GetCharacterMovement(); }

	UFUNCTION(BlueprintCallable, Category = Update)
	void UpdateAimingData();

	UFUNCTION(BlueprintCallable, Category = Update)
	void UpdateCharacterStateData();
	
	
protected:
	virtual void NativeBeginPlay() override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

	// Value Section
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<ATACCharacterBase> Owner;

	//	Velocity Data
	UPROPERTY(BlueprintReadOnly, Category = Velocity)
	FVector WorldVelocity;
	

	//	Aiming Data
	
	UPROPERTY(BlueprintReadOnly, Category = Aiming)
	float AimPitch;

	UPROPERTY(BlueprintReadOnly, Category = Aiming)
	float AimYaw;
	
	UPROPERTY(BlueprintReadOnly, Category = Aiming)
	float AimingAlpha;

	UPROPERTY(BlueprintReadOnly, Category = State)
	uint8 bADSStateChange : 1;

	UPROPERTY(BlueprintReadOnly, Category = State)
	uint8 bWasADSLastUpdate : 1;

	//	State Data
	UPROPERTY(BlueprintReadOnly, Category = State)
	uint8 bIsCrouching : 1;

	UPROPERTY(BlueprintReadOnly, Category = State)
	uint8 bIsSprint : 1;

	UPROPERTY(BlueprintReadOnly, Category = State)
	uint8 bIsJumping : 1;
	
	UPROPERTY(BlueprintReadOnly, Category = State)
	uint8 bIsFalling : 1;
	
	UPROPERTY(BlueprintReadOnly, Category = State)
	uint8 bIsOnGround : 1;

	UPROPERTY(BlueprintReadOnly, Category = State)
	uint8 bCrouchStateChange : 1;

	UPROPERTY(BlueprintReadOnly, Category = State)
	float TimeToJumpApex;
	
};