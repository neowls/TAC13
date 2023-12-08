// Fill out your copyright notice in the Description page of Project Settings.

#include "TACArmAnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Character/TACCharacterPlayer.h"
#include "Kismet/KismetMathLibrary.h"

UTACArmAnimInstance::UTACArmAnimInstance()
{
	AimingAlpha = 0.0f;
	bInterpAiming = false;
}

void UTACArmAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	OwnerPlayer = CastChecked<ATACCharacterPlayer>(Owner);
	SetNewSight();
}

void UTACArmAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if(!Owner) return;
	if(bInterpAiming) InterpAiming();
	if(bADSStateChange)
	{
		bInterpAiming = true;
	}
}


void UTACArmAnimInstance::SetNewSight()
{
	if(!OwnerPlayer) return;
	FTimerHandle TSetSightTransform;
	FTimerHandle TSetRelativeHandTransform;
	GetWorld()->GetTimerManager().SetTimer(TSetSightTransform,this, &UTACArmAnimInstance::SetSightTransform, .1f, false);
	GetWorld()->GetTimerManager().SetTimer(TSetRelativeHandTransform,this, &UTACArmAnimInstance::SetRelativeHandTransform, .1f, false);
}

void UTACArmAnimInstance::SetSightTransform()
{
	const FTransform CamTransform = OwnerPlayer->GetCameraComponent()->GetComponentTransform();
	const FTransform MeshTransform = OwnerPlayer->GetArmMesh()->GetComponentTransform();
	
	SightTransform = UKismetMathLibrary::MakeRelativeTransform(CamTransform,MeshTransform);

	SightTransform.SetLocation(SightTransform.GetLocation() + SightTransform.GetRotation().Vector() * 30.0f);
}

void UTACArmAnimInstance::SetRelativeHandTransform()
{
	if(!OwnerPlayer->GetCurrentSight()) return;
	const FTransform SightSocketTransform = OwnerPlayer->GetCurrentSight()->GetSocketTransform(FName("S_Aim"));
	const FTransform MeshTransform = OwnerPlayer->GetArmMesh()->GetSocketTransform(FName("hand_r"));
	RelativeHandTransform = UKismetMathLibrary::MakeRelativeTransform(SightSocketTransform, MeshTransform);
}

void UTACArmAnimInstance::SetLeftHandTransform()
{
	if(!OwnerPlayer->CurrentWeapon) return;
	const FTransform ArmMeshTransform = OwnerPlayer->GetArmMesh()->GetSocketTransform(FName("hand_r"));
	const FTransform LeftHandSocketTransform = OwnerPlayer->GetLeftHandTransform();
	LeftHandTransform  = UKismetMathLibrary::MakeRelativeTransform(LeftHandSocketTransform,ArmMeshTransform);
}

void UTACArmAnimInstance::InterpAiming()
{
	AimingAlpha = UKismetMathLibrary::FInterpTo(AimingAlpha, static_cast<float>(bWasADSLastUpdate), GetWorld()->GetDeltaSeconds(), 10.0f);
	
	if(AimingAlpha >= 1.0f || AimingAlpha <= 0.0f)
	{
		bInterpAiming = false;
	}
}
