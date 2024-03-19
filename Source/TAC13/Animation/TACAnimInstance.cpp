// Fill out your copyright notice in the Description page of Project Settings.

#include "TACAnimInstance.h"
#include "Character/TACCharacterPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"

UTACAnimInstance::UTACAnimInstance()
{
	bADSStateChange = false;
	bWasADSLastUpdate = false;
	AimingAlpha = 0.f;
	AimYaw = 0.f;
	AimPitch = 0.f;
}

void UTACAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	Owner = CastChecked<ATACCharacterBase>(TryGetPawnOwner());
	if(!Owner)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to Get Character"));
	}
}

void UTACAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
	if(!Owner) return;
	UpdateAimingData();
	UpdateCharacterStateData();
}

void UTACAnimInstance::UpdateAimingData()
{
	AimPitch = FRotator::NormalizeAxis(Owner->GetBaseAimRotation().Pitch);
	bADSStateChange = Owner->bIsADS != bWasADSLastUpdate;
	bWasADSLastUpdate = Owner->bIsADS;
}

void UTACAnimInstance::UpdateCharacterStateData()
{
	WorldVelocity = Owner->GetVelocity();
	bIsOnGround = Owner->GetCharacterMovement()->IsMovingOnGround();
	const bool WasCrouchingLastUpdate = bIsCrouching;
	bIsCrouching = Owner->GetCharacterMovement()->IsCrouching();
	bCrouchStateChange = (bIsCrouching != WasCrouchingLastUpdate);
	bIsSprint = Owner->bIsSprinting;
	bIsJumping = false;
	bIsFalling = false;
	//bHoldWeapon = Owner
	if(Owner->GetCharacterMovement()->MovementMode == MOVE_Falling)
	{
		if(WorldVelocity.Z > 0.f) bIsJumping = true;
		else bIsFalling = true;
	}
}