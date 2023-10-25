// Fill out your copyright notice in the Description page of Project Settings.

#include "TACAnimInstance.h"

#include "KismetAnimationLibrary.h"
#include "Character/TACCharacterPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"

UTACAnimInstance::UTACAnimInstance()
{
	bIsMoving = false;
	bIsSprint = false;
	bIsJumping = false;
	bIsFalling = false;
	bIsAiming = false;
	bInterpAiming = false;
}

void UTACAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	Character = CastChecked<ATACCharacterPlayer>(TryGetPawnOwner());
	if(!Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to Get Character"));
	}
}

void UTACAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if(!Character) return;
	SetMovement();
}

void UTACAnimInstance::SetMovement()
{
	Velocity = Character->GetVelocity();
	GroundSpeed = Velocity.Size2D();
	bIsMoving = GroundSpeed > 1.f;
	bIsSprint = false;
	Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, Character->GetActorRotation());
	bIsFalling = Character->GetCharacterMovement()->IsFalling();
	bIsJumping = bIsFalling & (Velocity.Z > 100.f);
	AimPitch = Character->GetBaseAimRotation().GetNormalized().Pitch;
}
