// Fill out your copyright notice in the Description page of Project Settings.

#include "TACAnimInstance.h"

#include "KismetAnimationLibrary.h"
#include "TAC13.h"
#include "Character/TACCharacterMovementComponent.h"
#include "Character/TACCharacterPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

UTACAnimInstance::UTACAnimInstance()
{
	bIsSprint = false;
	bIsJumping = false;
	bIsFalling = false;
	bADSStateChange = false;
	bIsADS = false;
	bIsCrouching = false;
	bIsFirstUpdate = true;
	bEnableRootYawOffset = true;
	RootYawOffsetAngleClamp = {-120.0f, 100.0f};
	RootYawOffsetAngleClampCrouched = {-90.0f, 80.0f};
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
	UpdateLocationData(DeltaSeconds);
	UpdateRotationData(DeltaSeconds);
	UpdateVelocityData();
	UpdateAccelerationData();
	UpdateCharacterStateData();
	UpdateRootYawOffset(DeltaSeconds);
	UpdateAimingData();
	UpdateJumpFallData(DeltaSeconds);
	UpdateGroundData();
	//if(Owner->IsLocallyControlled())UpdateDisplayLog();
	bIsFirstUpdate = false;
}

void UTACAnimInstance::SetRootYawOffset(float InRootYawOffset)
{
	if(bEnableRootYawOffset)
	{
		const FVector2D RootYawOffsetAngleClampTemp = (bIsCrouching ? RootYawOffsetAngleClamp : RootYawOffsetAngleClampCrouched);
		const bool SelectionBool =  RootYawOffsetAngleClampTemp.X == RootYawOffsetAngleClampTemp.Y;
		const float TempAngle = UKismetMathLibrary::NormalizeAxis(InRootYawOffset);
		RootYawOffset = SelectionBool ?  TempAngle : UKismetMathLibrary::ClampAngle(TempAngle, RootYawOffsetAngleClampTemp.X, RootYawOffsetAngleClampTemp.Y);
		AimYaw = RootYawOffset * -1.0f;
	}
	else
	{
		RootYawOffset = 0.0f;
		AimYaw = 0.0f;
	}
}

void UTACAnimInstance::ProcessTurnYawCurve()
{
	const float PreviousTurnYawCurveValue = TurnYawCurveValue;
	const float TurnYawWeight = GetCurveValue("TurnYawWeight");
	const float RemainingTurnYaw = GetCurveValue("RemainingTurnYaw");
	if(FMath::IsNearlyEqual(TurnYawWeight, 0,  0.0001))
	{
		TurnYawCurveValue = 0.0f;
	}
	else
	{
		TurnYawCurveValue = RemainingTurnYaw / TurnYawWeight;
		if(PreviousTurnYawCurveValue != 0.0f)
		{
			const float InRootYawOffset = RootYawOffset - (TurnYawCurveValue - PreviousTurnYawCurveValue);
			SetRootYawOffset(InRootYawOffset);
		}
	}
}

void UTACAnimInstance::UpdateDisplayLog()
{
	int LogKey = 0;
	LOG_SCREEN(LogKey++, TEXT("World Rotation: %s"), *WorldRotation.ToString());
	LOG_SCREEN(LogKey++, TEXT("World Location: %s"), *WorldLocation.ToString());
	LOG_SCREEN(LogKey++, TEXT("World Velocity : %s"), *WorldVelocity.ToString());	LOG_SCREEN(LogKey++, TEXT("Local Velocity2D : %s"), *LocalVelocity2D.ToString());
	LOG_SCREEN(LogKey++, TEXT("LocalVelocity Direction Angle : %f"), LocalVelocityDirectionAngle);
	LOG_SCREEN(LogKey++, TEXT("LocalVelocity Direction Angle With Offset : %f"), LocalVelocityDirectionAngleWithOffset);
	LOG_SCREEN(LogKey++, TEXT("Pivot Direction2D : %s"), *PivotDirection2D.ToString());
	LOG_SCREEN(LogKey++, TEXT("Local Acceleration2D : %s"), *LocalAcceleration2D.ToString());
	LOG_SCREEN(LogKey++, TEXT("Yaw Delta SinceLast Update : %f"), YawDeltaSinceLastUpdate);
	LOG_SCREEN(LogKey++, TEXT("Yaw Delta Speed : %f"), YawDeltaSpeed);
	LOG_SCREEN(LogKey++, TEXT("GroundSpeed : %f"), GroundSpeed);
	LOG_SCREEN(LogKey++, TEXT("Displacement Since Last Update: %f"), DisplacementSinceLastUpdate);
	LOG_SCREEN(LogKey++, TEXT("Displacement Speed : %f"), DisplacementSpeed);
	LOG_SCREEN(LogKey++, TEXT("Has Velocity : %s"), bHasVelocity ? TEXT("True") : TEXT("False"));
	LOG_SCREEN(LogKey++, TEXT("Has Acceleration : %s"), bHasAcceleration ? TEXT("True") : TEXT("False"));
	LOG_SCREEN(LogKey++, TEXT("Is Jumping : %s"), bIsJumping ? TEXT("True") : TEXT("False"));
	LOG_SCREEN(LogKey++, TEXT("Is Crouching : %s"), bIsCrouching ? TEXT("True") : TEXT("False"));
	LOG_SCREEN(LogKey++, TEXT("Is Sprint : %s"), bIsSprint ? TEXT("True") : TEXT("False"));
	LOG_SCREEN(LogKey++, TEXT("Is ADS : %s"), bIsADS ? TEXT("True") : TEXT("False"));
}

void UTACAnimInstance::UpdateLocationData(float DeltaTime)
{
	DisplacementSinceLastUpdate = (Owner->GetActorLocation() - WorldLocation).Size2D();
	WorldLocation = Owner->GetActorLocation();
	DisplacementSpeed = UKismetMathLibrary::SafeDivide(DisplacementSinceLastUpdate, DeltaTime);
	if(bIsFirstUpdate)
	{
		DisplacementSinceLastUpdate = 0.0f;
		DisplacementSpeed = 0.0f;
	}
}
	
void UTACAnimInstance::UpdateRotationData(float DeltaTime)
{
	YawDeltaSinceLastUpdate = Owner->GetActorRotation().Yaw - WorldRotation.Yaw;
	YawDeltaSpeed = UKismetMathLibrary::SafeDivide(YawDeltaSinceLastUpdate, DeltaTime);
	WorldRotation = Owner->GetActorRotation();
	if(bIsFirstUpdate)
	{
		YawDeltaSinceLastUpdate = 0.0f;
	}
	
}

void UTACAnimInstance::UpdateVelocityData()
{
	WorldVelocity = Owner->GetVelocity();
	const FVector WorldVelocity2D = WorldVelocity * FVector(1.0f, 1.0f, 0.f);
	LocalVelocity2D = WorldRotation.UnrotateVector(WorldVelocity2D);
	LocalVelocityDirectionAngle = UKismetAnimationLibrary::CalculateDirection(WorldVelocity2D, WorldRotation);
	LocalVelocityDirectionAngleWithOffset = LocalVelocityDirectionAngle - RootYawOffset;
	bHasVelocity = !UKismetMathLibrary::NearlyEqual_FloatFloat(UKismetMathLibrary::VSizeXYSquared(LocalVelocity2D), 0.0f);
	GroundSpeed = WorldVelocity2D.Length();

	
}

void UTACAnimInstance::UpdateAccelerationData()
{
	const FVector WorldAcceleration2D = Owner->GetCharacterMovement()->GetCurrentAcceleration() * FVector(1.0f,1.0f,0.0f);
	LocalAcceleration2D = WorldRotation.UnrotateVector(WorldAcceleration2D);
	bHasAcceleration = !UKismetMathLibrary::NearlyEqual_FloatFloat(LocalAcceleration2D.SizeSquared2D(), 0.0);
	PivotDirection2D = UKismetMathLibrary::VLerp(PivotDirection2D, WorldAcceleration2D.GetSafeNormal(0.0001), 0.5f).GetSafeNormal(0.0001);
}

void UTACAnimInstance::UpdateCharacterStateData()
{
	bIsOnGround = Owner->GetCharacterMovement()->IsMovingOnGround();
	const bool WasCrouchingLastUpdate = bIsCrouching;
	bIsCrouching = Owner->GetCharacterMovement()->IsCrouching();
	bCrouchStateChange = (bIsCrouching != WasCrouchingLastUpdate);
	bADSStateChange = Owner->bIsADS != bWasADSLastUpdate;
	bWasADSLastUpdate = Owner->bIsADS;
	bIsSprint = Owner->bIsSprinting;
	bIsJumping = false;
	bIsFalling = false;
	if(Owner->GetCharacterMovement()->MovementMode == MOVE_Falling)
	{
		if(WorldVelocity.Z > 0.f) bIsJumping = true;
		else bIsFalling = true;
	}
}

void UTACAnimInstance::UpdateRootYawOffset(float DeltaTime)
{
	if(RootYawOffsetMode == EAnim_RootYawOffsetMode::Accumulate)
	{
		SetRootYawOffset(RootYawOffset - YawDeltaSinceLastUpdate);
	}
	if(RootYawOffsetMode == EAnim_RootYawOffsetMode::BlendOut)
	{
		FFloatSpringState RootYawOffsetSpringState;
		SetRootYawOffset(UKismetMathLibrary::FloatSpringInterp(RootYawOffset, 0.0f, RootYawOffsetSpringState, 80.0f, 1.0f, DeltaTime, 1.0f, 0.5f));
	}
	RootYawOffsetMode = EAnim_RootYawOffsetMode::BlendOut;
}

void UTACAnimInstance::UpdateAimingData()
{
	AimPitch = FRotator::NormalizeAxis(Owner->GetBaseAimRotation().Pitch);
	bIsADS = Owner->bIsADS;
}

void UTACAnimInstance::UpdateJumpFallData(float DeltaTime)
{
	if(bIsJumping)
	{
		TimeToJumpApex = (0.0f - WorldVelocity.Z)/Owner->GetMovementComponent()->GetGravityZ();
	}
	else TimeToJumpApex = 0.0f;

	if(bIsFalling) TimeFalling += DeltaTime;
	else if(bIsJumping) TimeFalling = 0.0f;
}


void UTACAnimInstance::UpdateGroundData()
{
	GroundDistance = Owner->GetTACCharacterMovement()->GetGroundDistance();
}
