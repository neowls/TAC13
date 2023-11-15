// Fill out your copyright notice in the Description page of Project Settings.

#include "TACAnimInstance.h"

#include "KismetAnimationLibrary.h"
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
	UpdateRotationData();
	UpdateVelocityData();
	UpdateAccelerationData();
	UpdateWallDetectionHeuristic();
	UpdateCharacterStateData();
	UpdateBlendWeightData(DeltaSeconds);
	UpdateRootYawOffset(DeltaSeconds);
	UpdateAimingData();
	UpdateJumpFallData(DeltaSeconds);
	UpdateGroundData();
	
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

EAnim_CardinalDirection UTACAnimInstance::SelectCardinalDirectionFromAngle(float Angle, float DeadZone, EAnim_CardinalDirection CurrentDirection, bool UseCurrentDirection)
{
	const float ABSAngle = abs(Angle);
	float BwdDeadZone = DeadZone;
	float FwdDeadZone = DeadZone;
	if(UseCurrentDirection)
	{
		switch (CurrentDirection)
		{
			case EAnim_CardinalDirection::Forward:
				FwdDeadZone *= 2.0f;
				break;
			case EAnim_CardinalDirection::Backward:
				BwdDeadZone *= 2.0f;
				break;
			case EAnim_CardinalDirection::Left:
				break;
			case EAnim_CardinalDirection::Right:
				break;
			default: ;
		}
	}
	if(ABSAngle <= 45.0f + FwdDeadZone)	return EAnim_CardinalDirection::Forward;
	if(ABSAngle <= 135.0f - BwdDeadZone) return EAnim_CardinalDirection::Backward;
	if(Angle > 0.0f) return EAnim_CardinalDirection::Right;
	return EAnim_CardinalDirection::Left;
}

EAnim_CardinalDirection UTACAnimInstance::GetOppositeCardinalDirection(EAnim_CardinalDirection CurrentDirection)
{
	switch (CurrentDirection)
	{
		case EAnim_CardinalDirection::Forward:
			return EAnim_CardinalDirection::Backward;
		case EAnim_CardinalDirection::Backward:
			return EAnim_CardinalDirection::Forward;
		case EAnim_CardinalDirection::Left:
			return EAnim_CardinalDirection::Right;
		case EAnim_CardinalDirection::Right:
			return EAnim_CardinalDirection::Left;
		default: return CurrentDirection;
	}
}

bool UTACAnimInstance::IsMovingPerpendicularToInitialPivot()
{
	const bool XDirection = (PivotInitialDirection == EAnim_CardinalDirection::Forward || PivotInitialDirection == EAnim_CardinalDirection::Backward) && (LocalVelocityDirection == EAnim_CardinalDirection::Forward || LocalVelocityDirection == EAnim_CardinalDirection::Backward);
	const bool YDirection = (PivotInitialDirection == EAnim_CardinalDirection::Left || PivotInitialDirection == EAnim_CardinalDirection::Right) && (LocalVelocityDirection == EAnim_CardinalDirection::Left || LocalVelocityDirection == EAnim_CardinalDirection::Right);

	return XDirection || YDirection;
}

bool UTACAnimInstance::ShouldEnableControlRig()
{
	return (GetCurveValue(TEXT("DisableLegIK")) <= 0.0f) && !bUseFootPlacement;
}

UCharacterMovementComponent* UTACAnimInstance::GetCharacterMovementComponent()
{
	return Owner->GetTACCharacterMovement();
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

void UTACAnimInstance::UpdateRotationData()
{
	YawDeltaSinceLastUpdate = Owner->GetActorRotation().Yaw - WorldRotation.Yaw;
	YawDeltaSpeed = UKismetMathLibrary::SafeDivide(YawDeltaSinceLastUpdate, GetDeltaSeconds());
	WorldRotation = Owner->GetActorRotation();
	if(bIsFirstUpdate)
	{
		YawDeltaSinceLastUpdate = 0.0f;
	}
}

void UTACAnimInstance::UpdateVelocityData()
{
	const bool WasMovingLastUpdate = !UKismetMathLibrary::Vector_IsZero(LocalVelocity2D);
	WorldVelocity = Owner->GetVelocity();
	const FVector WorldVelocity2D = WorldVelocity * FVector(1.0f, 1.0f, 0.f);
	LocalVelocity2D = WorldRotation.UnrotateVector(WorldVelocity2D);
	LocalVelocityDirectionAngle = UKismetAnimationLibrary::CalculateDirection(WorldVelocity2D, WorldRotation);
	LocalVelocityDirectionAngleWithOffset = LocalVelocityDirectionAngle - RootYawOffset;
	LocalVelocityDirection = SelectCardinalDirectionFromAngle(LocalVelocityDirectionAngleWithOffset, CardinalDirectionDeadZone, LocalVelocityDirection, WasMovingLastUpdate);
	LocalVelocityDirectionNoOffset = SelectCardinalDirectionFromAngle(LocalVelocityDirectionAngle, CardinalDirectionDeadZone, LocalVelocityDirectionNoOffset, WasMovingLastUpdate);
	bHasVelocity = !UKismetMathLibrary::NearlyEqual_FloatFloat(UKismetMathLibrary::VSizeXYSquared(LocalVelocity2D), 0.0f);
	GroundSpeed = WorldVelocity2D.Length();
}

void UTACAnimInstance::UpdateAccelerationData()
{
	const FVector WorldAcceleration2D = Owner->GetCharacterMovement()->GetCurrentAcceleration() * FVector(1.0f,1.0f,0.0f);
	LocalAcceleration2D = WorldRotation.UnrotateVector(WorldAcceleration2D);
	bHasAcceleration = !UKismetMathLibrary::NearlyEqual_FloatFloat(LocalAcceleration2D.SizeSquared2D(), 0.0);
	PivotDirection2D = UKismetMathLibrary::VLerp(PivotDirection2D, WorldAcceleration2D.GetSafeNormal(0.0001), 0.5f).GetSafeNormal(0.0001);
	CardinalDirectionFromAcceleration = GetOppositeCardinalDirection(SelectCardinalDirectionFromAngle(UKismetAnimationLibrary::CalculateDirection(PivotDirection2D, WorldRotation), CardinalDirectionDeadZone, EAnim_CardinalDirection::Forward, false));
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

void UTACAnimInstance::UpdateWallDetectionHeuristic()
{
	bIsRunningIntoWall = (LocalAcceleration2D.Size2D() > 0.1f && LocalVelocity2D.Size2D() < 200.0f &&
		(UKismetMathLibrary::InRange_FloatFloat(UKismetMathLibrary::Dot_VectorVector(LocalAcceleration2D.GetSafeNormal(), LocalVelocity2D.GetSafeNormal()), -0.6f, 0.6f)));
}

void UTACAnimInstance::UpdateBlendWeightData(float DeltaTime)
{
	UpperbodyDynamicAdditiveWeight = (IsAnyMontagePlaying() && bIsOnGround) ? 1.0f : UKismetMathLibrary::FInterpTo(UpperbodyDynamicAdditiveWeight, 0.0f, DeltaTime, 6.0f);
}

void UTACAnimInstance::UpdateGroundData()
{
	GroundDistance = Owner->GetTACCharacterMovement()->GetGroundDistance();
}
