// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/TACBodyAnimInstance.h"
#include "KismetAnimationLibrary.h"
#include "TAC13.h"
#include "Kismet/KismetMathLibrary.h"
#include "Character/TACCharacterPlayer.h"
#include "Character/TACCharacterMovementComponent.h"

UTACBodyAnimInstance::UTACBodyAnimInstance()
{
	bIsSprint = false;
	bIsJumping = false;
	bIsFalling = false;
	bIsCrouching = false;
	bIsFirstUpdate = true;
	bEnableRootYawOffset = true;
	RootYawOffsetAngleClamp = {-120.0f, 100.0f};
	RootYawOffsetAngleClampCrouched = {-90.0f, 80.0f};
}


void UTACBodyAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
	if(!Owner) return;
	UpdateLocationData(DeltaSeconds);
	UpdateRotationData(DeltaSeconds);
	UpdateVelocityData();
	UpdateAccelerationData();
	UpdateCharacterStateData();
	UpdateRootYawOffset(DeltaSeconds);
	UpdateJumpFallData(DeltaSeconds);
	UpdateGroundData();
	//UpdateDisplayLog();
	bIsFirstUpdate = false;
}

void UTACBodyAnimInstance::SetRootYawOffset(float InRootYawOffset)
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

void UTACBodyAnimInstance::ProcessTurnYawCurve()
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

void UTACBodyAnimInstance::UpdateDisplayLog()
{
	int LogKey = 0;
	LOG_SCREEN(LogKey++, TEXT("Yaw Delta Speed : %f"), YawDeltaSpeed);
	LOG_SCREEN(LogKey++, TEXT("GroundSpeed : %f"), GroundSpeed);
	LOG_SCREEN(LogKey++, TEXT("Has Velocity : %s"), bHasVelocity ? TEXT("True") : TEXT("False"));
	LOG_SCREEN(LogKey++, TEXT("Is Jumping : %s"), bIsJumping ? TEXT("True") : TEXT("False"));
	LOG_SCREEN(LogKey++, TEXT("Is Crouching : %s"), bIsCrouching ? TEXT("True") : TEXT("False"));
	LOG_SCREEN(LogKey, TEXT("Is Sprint : %s"), bIsSprint ? TEXT("True") : TEXT("False"));
	LOG_SCREEN(LogKey, TEXT("Is ADS : %s"), bWasADSLastUpdate ? TEXT("True") : TEXT("False"));
}

void UTACBodyAnimInstance::UpdateLocationData(float DeltaTime)
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
	
void UTACBodyAnimInstance::UpdateRotationData(float DeltaTime)
{
	YawDeltaSinceLastUpdate = Owner->GetActorRotation().Yaw - WorldRotation.Yaw;
	YawDeltaSpeed = UKismetMathLibrary::SafeDivide(YawDeltaSinceLastUpdate, DeltaTime);
	WorldRotation = Owner->GetActorRotation();
	if(bIsFirstUpdate)
	{
		YawDeltaSinceLastUpdate = 0.0f;
	}
	
}

void UTACBodyAnimInstance::UpdateVelocityData()
{
	WorldVelocity = Owner->GetVelocity();
	const FVector WorldVelocity2D = WorldVelocity * FVector(1.0f, 1.0f, 0.f);
	LocalVelocity2D = WorldRotation.UnrotateVector(WorldVelocity2D);
	LocalVelocityDirectionAngle = UKismetAnimationLibrary::CalculateDirection(WorldVelocity2D, WorldRotation);
	LocalVelocityDirectionAngleWithOffset = LocalVelocityDirectionAngle - RootYawOffset;
	bHasVelocity = !UKismetMathLibrary::NearlyEqual_FloatFloat(UKismetMathLibrary::VSizeXYSquared(LocalVelocity2D), 0.0f);
	GroundSpeed = WorldVelocity2D.Length();

	
}

void UTACBodyAnimInstance::UpdateAccelerationData()
{
	const FVector WorldAcceleration2D = Owner->GetCharacterMovement()->GetCurrentAcceleration() * FVector(1.0f,1.0f,0.0f);
	LocalAcceleration2D = WorldRotation.UnrotateVector(WorldAcceleration2D);
	bHasAcceleration = !UKismetMathLibrary::NearlyEqual_FloatFloat(LocalAcceleration2D.SizeSquared2D(), 0.0);
	PivotDirection2D = UKismetMathLibrary::VLerp(PivotDirection2D, WorldAcceleration2D.GetSafeNormal(0.0001), 0.5f).GetSafeNormal(0.0001);
}

void UTACBodyAnimInstance::UpdateCharacterStateData()
{
	bIsOnGround = Owner->GetCharacterMovement()->IsMovingOnGround();
	const bool WasCrouchingLastUpdate = bIsCrouching;
	bIsCrouching = Owner->GetCharacterMovement()->IsCrouching();
	bCrouchStateChange = (bIsCrouching != WasCrouchingLastUpdate);
	bIsSprint = Owner->bIsSprinting;
	bIsJumping = false;
	bIsFalling = false;
	if(Owner->GetCharacterMovement()->MovementMode == MOVE_Falling)
	{
		if(WorldVelocity.Z > 0.f) bIsJumping = true;
		else bIsFalling = true;
	}
}

void UTACBodyAnimInstance::UpdateRootYawOffset(float DeltaTime)
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

void UTACBodyAnimInstance::UpdateJumpFallData(float DeltaTime)
{
	if(bIsJumping)
	{
		TimeToJumpApex = (0.0f - WorldVelocity.Z)/Owner->GetMovementComponent()->GetGravityZ();
	}
	else TimeToJumpApex = 0.0f;

	if(bIsFalling) TimeFalling += DeltaTime;
	else if(bIsJumping) TimeFalling = 0.0f;
}


void UTACBodyAnimInstance::UpdateGroundData()
{
	GroundDistance = Owner->GetTACCharacterMovement()->GetGroundDistance();
}
