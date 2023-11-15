// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TACCharacterMovementComponent.h"

#include "TAC13.h"
#include "Components/CapsuleComponent.h"
#include "Character/TACCharacterBase.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameStateBase.h"

UTACCharacterMovementComponent::UTACCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//	Prone Section	
	MaxAccelerationProned = 256.f;
	MaxWalkSpeedProned = 168.f;
	BrakingDecelerationProned = 512.f;
	GroundFrictionProned = 3.f;
	BrakingFrictionProned = 1.f;
	PronedRadius = 40.f;
	PronedHalfHeight = 40.f;
	ProneLockDuration = 0.5f;

	//	Sprint Section
	MaxAccelerationSprinting = 1024.f;
	MaxWalkSpeedSprinting = 600.f;
	BrakingDecelerationSprinting = 512.f;
	GroundFrictionSprinting = 8.f;
	BrakingFrictionSprinting = 4.f;
	VelocityCheckMitigatorSprinting = 0.98f;
	MaxBaseSpeed = MaxWalkSpeed;
}

const float UTACCharacterMovementComponent::GetGroundDistance()
{
	const float GroundTraceDistance = 100000.0f;
	if (MovementMode == MOVE_Walking)
	{
		return 0.0f;
	}
	
	const UCapsuleComponent* CapsuleComp = CharacterOwner->GetCapsuleComponent();
	check(CapsuleComp);

	const float CapsuleHalfHeight = CapsuleComp->GetUnscaledCapsuleHalfHeight();
	const ECollisionChannel CollisionChannel = (UpdatedComponent ? UpdatedComponent->GetCollisionObjectType() : ECC_Pawn);
	const FVector TraceStart(GetActorLocation());
	const FVector TraceEnd(TraceStart.X, TraceStart.Y, (TraceStart.Z - GroundTraceDistance - CapsuleHalfHeight));

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(CharacterMovementComponent_GetGroundInfo), false, CharacterOwner);
	FCollisionResponseParams ResponseParam;
	InitCollisionParams(QueryParams, ResponseParam);

	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, CollisionChannel, QueryParams, ResponseParam);

	if(MovementMode == MOVE_Walking) return 0.0f;
	if(HitResult.bBlockingHit) return FMath::Max((HitResult.Distance - CapsuleHalfHeight), 0.0f);
	
	return 0.0f;
}

void UTACCharacterMovementComponent::UnCrouch(bool bClientSimulation)
{
	Super::UnCrouch(bClientSimulation);
	if(CharacterOwner->bIsCrouched) bWantsToCrouch = true;
}

bool UTACCharacterMovementComponent::HasValidData() const
{
	return Super::HasValidData() && IsValid(TACCharacterOwner);
}

void UTACCharacterMovementComponent::PostLoad()
{
	Super::PostLoad();

	TACCharacterOwner = Cast<ATACCharacterBase>(PawnOwner);
}

void UTACCharacterMovementComponent::SetUpdatedComponent(USceneComponent* NewUpdatedComponent)
{
	Super::SetUpdatedComponent(NewUpdatedComponent);

	TACCharacterOwner = Cast<ATACCharacterBase>(PawnOwner);
}

float UTACCharacterMovementComponent::GetMaxAcceleration() const
{
	if (IsMovingOnGround())
	{
		if(IsProned())	return MaxAccelerationProned;
		if(IsSprinting()) return MaxAccelerationSprinting;
	}
	
	return Super::GetMaxAcceleration();
}

float UTACCharacterMovementComponent::GetMaxSpeed() const
{
	if (IsMovingOnGround())
	{
		if(IsProned()) return MaxWalkSpeedProned;
		if(IsSprinting()) return MaxWalkSpeedSprinting;
	}
	return Super::GetMaxSpeed();
}

float UTACCharacterMovementComponent::GetMaxBrakingDeceleration() const
{
	if (IsMovingOnGround())
	{
		if(IsProned()) return BrakingDecelerationProned;
		if(IsSprinting()) return BrakingDecelerationSprinting;
	}
	return Super::GetMaxBrakingDeceleration();
}

void UTACCharacterMovementComponent::CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration)
{
	if(IsMovingOnGround())
	{
		if(IsProned())	Friction = GroundFrictionProned;
		else if(IsSprinting()) Friction = GroundFrictionSprinting;
	}
	Super::CalcVelocity(DeltaTime, Friction, bFluid, BrakingDeceleration);
}

void UTACCharacterMovementComponent::ApplyVelocityBraking(float DeltaTime, float Friction, float BrakingDeceleration)
{
	if (IsMovingOnGround())
	{
		if(IsProned())	Friction = (bUseSeparateBrakingFriction ? BrakingFrictionProned : GroundFrictionProned);
		else if(IsSprinting()) Friction = (bUseSeparateBrakingFriction ? BrakingFrictionSprinting : GroundFrictionSprinting); 
	}
	Super::ApplyVelocityBraking(DeltaTime, Friction, BrakingDeceleration);
}


bool UTACCharacterMovementComponent::IsSprintingAtSpeed() const
{
	if (!IsSprinting())
	{
		return false;
	}

	// When moving on ground we want to factor moving uphill or downhill so variations in terrain
	// aren't culled from the check. When falling, we don't want to factor fall velocity, only lateral
	const float Vel = IsMovingOnGround() ? Velocity.SizeSquared() : Velocity.SizeSquared2D();
	const float WalkSpeed = IsCrouching() ? MaxWalkSpeedCrouched : MaxWalkSpeed;

	// When struggling to surpass walk speed, which can occur with heavy rotation and low acceleration, we
	// mitigate the check so there isn't a constant re-entry that can occur as an edge case
	return Vel >= (WalkSpeed * WalkSpeed * VelocityCheckMitigatorSprinting);
}

bool UTACCharacterMovementComponent::IsSprinting() const
{
	return TACCharacterOwner && TACCharacterOwner->bIsSprinting;
}

void UTACCharacterMovementComponent::Sprint(bool bClientSimulation)
{
	if (!HasValidData())
	{
		return;
	}

	if (!bClientSimulation && !CanSprintInCurrentState())
	{
		return;
	}

	if (!bClientSimulation)
	{
		MaxWalkSpeed = MaxWalkSpeedSprinting;
		TACCharacterOwner->bIsSprinting = true;
	}
	TACCharacterOwner->OnStartSprint();
}

void UTACCharacterMovementComponent::UnSprint(bool bClientSimulation)
{
	if (!HasValidData())
	{
		return;
	}

	if (!bClientSimulation)
	{
		MaxWalkSpeed = MaxBaseSpeed;
		TACCharacterOwner->bIsSprinting = false;
	}
	TACCharacterOwner->OnEndSprint();
}

bool UTACCharacterMovementComponent::CanSprintInCurrentState() const
{
	if (!UpdatedComponent || UpdatedComponent->IsSimulatingPhysics())
	{
		return false;
	}

	if (!IsFalling() && !IsMovingOnGround())
	{
		return false;
	}
	
	if (!IsSprintWithinAllowableInputAngle())
	{
		return false;
	}

	return true;
}

bool UTACCharacterMovementComponent::IsSprintWithinAllowableInputAngle() const
{
	// This check ensures that we are not sprinting backward or sideways, while allowing leeway 
	// This angle allows sprinting when holding forward, forward left, forward right
	// but not left or right or backward)
	static constexpr float MaxSprintInputDegrees = 50.f;
	static constexpr float MaxSprintInputNormal = 0.64278732;  // cos(rad(MaxSprintInputDegrees))

	if constexpr (MaxSprintInputDegrees > 0.f)
	{
		const float Dot = (GetCurrentAcceleration().GetSafeNormal2D() | UpdatedComponent->GetForwardVector());
		if (Dot < MaxSprintInputNormal)
		{
			return false;
		}
	}
	return true;
}

bool UTACCharacterMovementComponent::CanWalkOffLedges() const
{
	if (!bCanWalkOffLedgesWhenProned && IsProned())
	{
		return false;
	}
	
	return Super::CanWalkOffLedges();
}

bool UTACCharacterMovementComponent::CanAttemptJump() const
{
	return Super::CanAttemptJump() && !bWantsToProne;
}

float UTACCharacterMovementComponent::GetTimestamp() const
{
	if(CharacterOwner->GetLocalRole() == ROLE_Authority)
	{
		if (CharacterOwner->IsLocallyControlled())
		{
			// Server owned character (Server Local Play) (Local : ROLE_Authority / Remote : ROLE_AutonomousProxy) 
			return GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
		}
		// Server remote character (Server Remote Play) (Local : ROLE_Authority / Remote : ROLE_AutonomousProxy)
		const FNetworkPredictionData_Server_Character* ServerData = GetPredictionData_Server_Character();
		return ServerData->CurrentClientTimeStamp;
	}

	// Client owned character (Client Local Play) (Local : ROLE_AutonomousProxy / Remote : ROLE_Authority) (Client Remote Play) (Local : ROLE_SimulatedProxy / Remote : ROLE_Authority) 
	// const FNetworkPredictionData_Client_Character* ClientData = GetPredictionData_Client_Character();
	return GetWorld()->GetTimeSeconds(); //ClientData->CurrentTimeStamp;
}

bool UTACCharacterMovementComponent::IsProned() const
{
	return TACCharacterOwner && TACCharacterOwner->bIsProned;
}

void UTACCharacterMovementComponent::Prone(bool bClientSimulation)
{
	if (!HasValidData())
	{
		return;
	}

	if (!bClientSimulation && !CanProneInCurrentState())
	{
		return;
	}

	// See if collision is already at desired size.
	if (CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() == PronedHalfHeight &&
		CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleRadius() == PronedRadius)
	{
		if (!bClientSimulation)
		{
			TACCharacterOwner->bIsProned = true;
		}
		TACCharacterOwner->OnStartProne( 0.f, 0.f );
		SetProneLock(true);
		return;
	}

	if (bClientSimulation && CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)
	{
		// restore collision size before prone
		const ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
		CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight());
		bShrinkProxyCapsule = true;
	}

	// Change collision size to prone dimensions
	const float ComponentScale = CharacterOwner->GetCapsuleComponent()->GetShapeScale();
	const float OldUnscaledHalfHeight = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	const float OldUnscaledRadius = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleRadius();

	// Height is not allowed to be smaller than radius.
	const float ClampedPronedHalfHeight = FMath::Max3(0.f, PronedRadius, PronedHalfHeight);
	CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(PronedRadius, ClampedPronedHalfHeight);
	float HalfHeightAdjust = (OldUnscaledHalfHeight - ClampedPronedHalfHeight);
	float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;

	if( !bClientSimulation )
	{
		// Proned to a larger height? (this is rare)
		if (ClampedPronedHalfHeight > OldUnscaledHalfHeight)
		{
			FCollisionQueryParams CapsuleParams(SCENE_QUERY_STAT(ProneTrace), false, CharacterOwner);
			FCollisionResponseParams ResponseParam;
			InitCollisionParams(CapsuleParams, ResponseParam);
			const bool bEncroached = GetWorld()->OverlapBlockingTestByChannel(UpdatedComponent->GetComponentLocation() - FVector(0.f,0.f,ScaledHalfHeightAdjust), FQuat::Identity,
				UpdatedComponent->GetCollisionObjectType(), GetPawnCapsuleCollisionShape(SHRINK_None), CapsuleParams, ResponseParam);

			// If encroached, cancel
			if( bEncroached )
			{
				CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(OldUnscaledRadius, OldUnscaledHalfHeight);
				return;
			}
		}

		if (bCrouchMaintainsBaseLocation)
		{
			// Intentionally not using MoveUpdatedComponent, where a horizontal plane constraint would prevent the base of the capsule from staying at the same spot.
			UpdatedComponent->MoveComponent(FVector(0.f, 0.f, -ScaledHalfHeightAdjust), UpdatedComponent->GetComponentQuat(), true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
		}

		TACCharacterOwner->bIsProned = true;
	}

	// Our capsule is growing during prone, test for encroaching from radius
	FCollisionQueryParams CapsuleParams(SCENE_QUERY_STAT(ProneTrace), false, CharacterOwner);
	FCollisionResponseParams ResponseParam;
	InitCollisionParams(CapsuleParams, ResponseParam);
	FHitResult Hit;
	const FVector Start = UpdatedComponent->GetComponentLocation() - FVector(0.f,0.f,ScaledHalfHeightAdjust);
	const FVector End = UpdatedComponent->GetComponentLocation() - FVector(0.f,0.f,ScaledHalfHeightAdjust * 1.01f);
	if (GetWorld()->SweepSingleByChannel(Hit, Start, End, FQuat::Identity, UpdatedComponent->GetCollisionObjectType(), FCollisionShape::MakeCapsule(PronedRadius, PronedHalfHeight), CapsuleParams, ResponseParam))
	{
		if (Hit.bStartPenetrating)
		{
			HandleImpact(Hit);
			SlideAlongSurface(FVector::DownVector, 1.f, Hit.Normal, Hit, true);

			if (Hit.bStartPenetrating)
			{
				OnCharacterStuckInGeometry(&Hit);
			}
		}
	}
	
	bForceNextFloorCheck = true;

	SetProneLock(true);

	// OnStartProne takes the change from the Default size, not the current one (though they are usually the same).
	const float MeshAdjust = ScaledHalfHeightAdjust;
	const ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
	HalfHeightAdjust = (DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - ClampedPronedHalfHeight);
	ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;

	AdjustProxyCapsuleSize();
	TACCharacterOwner->OnStartProne( HalfHeightAdjust, ScaledHalfHeightAdjust );

	// Don't smooth this change in mesh position
	if ((bClientSimulation && CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy) || (IsNetMode(NM_ListenServer) && CharacterOwner->GetRemoteRole() == ROLE_AutonomousProxy))
	{
		FNetworkPredictionData_Client_Character* ClientData = GetPredictionData_Client_Character();
		if (ClientData)
		{
			ClientData->MeshTranslationOffset -= FVector(0.f, 0.f, MeshAdjust);
			ClientData->OriginalMeshTranslationOffset = ClientData->MeshTranslationOffset;
		}
	}
}

void UTACCharacterMovementComponent::UnProne(bool bClientSimulation)
{
	if (!HasValidData())
	{
		return;
	}
	
	if (IsProneLocked())
	{
		return;
	}

	ATACCharacterBase* DefaultCharacter = TACCharacterOwner->GetClass()->GetDefaultObject<ATACCharacterBase>();

	// See if collision is already at desired size.
	if (CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() == DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() &&
		CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleRadius() == DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius())
	{
		if (!bClientSimulation)
		{
			TACCharacterOwner->bIsProned = false;
		}
		TACCharacterOwner->OnEndProne( 0.f, 0.f );
		return;
	}

	const float CurrentPronedHalfHeight = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	const float ComponentScale = CharacterOwner->GetCapsuleComponent()->GetShapeScale();
	const float OldUnscaledHalfHeight = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	const float HalfHeightAdjust = DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - OldUnscaledHalfHeight;
	const float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;
	const FVector PawnLocation = UpdatedComponent->GetComponentLocation();

	// Grow to unproned size.
	check(CharacterOwner->GetCapsuleComponent());

	if( !bClientSimulation )
	{
		// Try to stay in place and see if the larger capsule fits. We use a slightly taller capsule to avoid penetration.
		const UWorld* MyWorld = GetWorld();
		constexpr float SweepInflation = UE_KINDA_SMALL_NUMBER * 10.f;
		FCollisionQueryParams CapsuleParams(SCENE_QUERY_STAT(ProneTrace), false, CharacterOwner);
		FCollisionResponseParams ResponseParam;
		InitCollisionParams(CapsuleParams, ResponseParam);

		// Compensate for the difference between current capsule size and standing size
		const FCollisionShape StandingCapsuleShape = GetPawnCapsuleCollisionShape(SHRINK_HeightCustom, -SweepInflation - ScaledHalfHeightAdjust); // Shrink by negative amount, so actually grow it.
		const ECollisionChannel CollisionChannel = UpdatedComponent->GetCollisionObjectType();
		bool bEncroached = true;

		if (!bCrouchMaintainsBaseLocation)
		{
			// Expand in place
			bEncroached = MyWorld->OverlapBlockingTestByChannel(PawnLocation, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);
		
			if (bEncroached)
			{
				// Try adjusting capsule position to see if we can avoid encroachment.
				if (ScaledHalfHeightAdjust > 0.f)
				{
					// Shrink to a short capsule, sweep down to base to find where that would hit something, and then try to stand up from there.
					float PawnRadius, PawnHalfHeight;
					CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleSize(PawnRadius, PawnHalfHeight);
					const float ShrinkHalfHeight = PawnHalfHeight - PawnRadius;
					const float TraceDist = PawnHalfHeight - ShrinkHalfHeight;
					const FVector Down = FVector(0.f, 0.f, -TraceDist);

					FHitResult Hit(1.f);
					const FCollisionShape ShortCapsuleShape = GetPawnCapsuleCollisionShape(SHRINK_HeightCustom, ShrinkHalfHeight);
					MyWorld->SweepSingleByChannel(Hit, PawnLocation, PawnLocation + Down, FQuat::Identity, CollisionChannel, ShortCapsuleShape, CapsuleParams);
					if (Hit.bStartPenetrating)
					{
						bEncroached = true;
					}
					else
					{
						// Compute where the base of the sweep ended up, and see if we can stand there
						const float DistanceToBase = (Hit.Time * TraceDist) + ShortCapsuleShape.Capsule.HalfHeight;
						const FVector NewLoc = FVector(PawnLocation.X, PawnLocation.Y, PawnLocation.Z - DistanceToBase + StandingCapsuleShape.Capsule.HalfHeight + SweepInflation + MIN_FLOOR_DIST / 2.f);
						bEncroached = MyWorld->OverlapBlockingTestByChannel(NewLoc, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);
						if (!bEncroached)
						{
							// Intentionally not using MoveUpdatedComponent, where a horizontal plane constraint would prevent the base of the capsule from staying at the same spot.
							UpdatedComponent->MoveComponent(NewLoc - PawnLocation, UpdatedComponent->GetComponentQuat(), false, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
						}
					}
				}
			}
		}
		else
		{
			// Expand while keeping base location the same.
			FVector StandingLocation = PawnLocation + FVector(0.f, 0.f, StandingCapsuleShape.GetCapsuleHalfHeight() - CurrentPronedHalfHeight);
			bEncroached = MyWorld->OverlapBlockingTestByChannel(StandingLocation, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);

			if (bEncroached)
			{
				if (IsMovingOnGround())
				{
					// Something might be just barely overhead, try moving down closer to the floor to avoid it.
					constexpr float MinFloorDist = UE_KINDA_SMALL_NUMBER * 10.f;
					if (CurrentFloor.bBlockingHit && CurrentFloor.FloorDist > MinFloorDist)
					{
						StandingLocation.Z -= CurrentFloor.FloorDist - MinFloorDist;
						bEncroached = MyWorld->OverlapBlockingTestByChannel(StandingLocation, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);
					}
				}				
			}

			if (!bEncroached)
			{
				// Commit the change in location.
				UpdatedComponent->MoveComponent(StandingLocation - PawnLocation, UpdatedComponent->GetComponentQuat(), false, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
				bForceNextFloorCheck = true;
			}
		}

		// If still encroached then abort.
		if (bEncroached)
		{
			return;
		}

		TACCharacterOwner->bIsProned = false;
	}	
	else
	{
		bShrinkProxyCapsule = true;
	}

	// Now call SetCapsuleSize() to cause touch/untouch events and actually grow the capsule
	CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight(), true);

	const float MeshAdjust = ScaledHalfHeightAdjust;
	AdjustProxyCapsuleSize();
	TACCharacterOwner->OnEndProne( HalfHeightAdjust, ScaledHalfHeightAdjust );

	// Don't smooth this change in mesh position
	if ((bClientSimulation && CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy) || (IsNetMode(NM_ListenServer) && CharacterOwner->GetRemoteRole() == ROLE_AutonomousProxy))
	{
		FNetworkPredictionData_Client_Character* ClientData = GetPredictionData_Client_Character();
		if (ClientData)
		{
			ClientData->MeshTranslationOffset += FVector(0.f, 0.f, MeshAdjust);
			ClientData->OriginalMeshTranslationOffset = ClientData->MeshTranslationOffset;
		}
	}
}

bool UTACCharacterMovementComponent::CanProneInCurrentState() const
{
	return (IsFalling() || IsMovingOnGround()) && UpdatedComponent && !UpdatedComponent->IsSimulatingPhysics();
}

void UTACCharacterMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	// Proxies get replicated crouch state.
	if (CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)
	{
		// Check for a change in Sprint state. Players toggle Sprint by changing bWantsToSprint.
		const bool bIsSprinting = IsSprinting();
		if (bIsSprinting && (!bWantsToSprint || !CanSprintInCurrentState()))
		{
			UnSprint(false);
		}
		else if (!bIsSprinting && bWantsToSprint && CanSprintInCurrentState())
		{
			Sprint(false);
		}
		
		// Check for a change in crouch state. Players toggle crouch by changing bWantsToCrouch.
		const bool bIsCrouching = IsCrouching();

		if (bIsCrouching && (!bWantsToCrouch || !CanCrouchInCurrentState()))
		{
			UnCrouch(false);
		}
		else if (!bIsCrouching && bWantsToCrouch && CanCrouchInCurrentState())
		{
			if (IsProned())
			{
				bWantsToProne = false;
				UnProne(false);
			}
			if (!IsProned())
			{
				// Potential prone lock
				Crouch(false);
			}
		}

		// Check for a change in Prone state. Players toggle Prone by changing bWantsToProne.
		const bool bIsProned = IsProned();
		if (bIsProned && (!bWantsToProne || !CanProneInCurrentState()))
		{
			UnProne(false);
		}
		else if (!bIsProned && bWantsToProne && CanProneInCurrentState())
		{
			if (IsCrouching())
			{
				bWantsToCrouch = false;
				UnCrouch(false);
			}
			Prone(false);
		}
	}

	// Check if prone lock timer has expired
	if (bProneLocked && !IsProneLockOnTimer())
	{
		SetProneLock(false);
	}
}

void UTACCharacterMovementComponent::UpdateCharacterStateAfterMovement(float DeltaSeconds)
{
	if (CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)
	{
		// UnSprint if no longer allowed to be Sprinting
		if (IsSprinting() && !CanSprintInCurrentState())
		{
			UnSprint(false);
		}
	}
	
	Super::UpdateCharacterStateAfterMovement(DeltaSeconds);

	// Proxies get replicated Prone state.
	if (CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)
	{
		// UnProne if no longer allowed to be Proned
		if (IsProned() && !CanProneInCurrentState())
		{
			UnProne(false);
		}
	}
}

bool UTACCharacterMovementComponent::ClientUpdatePositionAfterServerUpdate()
{
	const bool bRealProne = bWantsToProne;
	const bool bRealSprint = bWantsToSprint;
	const bool bResult = Super::ClientUpdatePositionAfterServerUpdate();
	bWantsToProne = bRealProne;
	bWantsToSprint = bRealSprint;

	return bResult;
}

void FSavedMove_Character_TAC::Clear()
{
	Super::Clear();

	bWantsToSprint = false;
	bWantsToProne = false;
	bProneLocked = false;
}

void FSavedMove_Character_TAC::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel,
	FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);

	bWantsToSprint = Cast<ATACCharacterBase>(C)->GetTACCharacterMovement()->bWantsToSprint;
	bWantsToProne = Cast<ATACCharacterBase>(C)->GetTACCharacterMovement()->bWantsToProne;
	bProneLocked = Cast<ATACCharacterBase>(C)->GetTACCharacterMovement()->bProneLocked;
	
}

void FSavedMove_Character_TAC::PrepMoveFor(ACharacter* C)
{
	Super::PrepMoveFor(C);

	Cast<ATACCharacterBase>(C)->GetTACCharacterMovement()->bProneLocked = bProneLocked;
	
}

uint8 FSavedMove_Character_TAC::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();

	if(bWantsToSprint)
	{
		Result |= FLAG_Custom_0;
	}
	
	if (bWantsToProne)
	{
		Result |= FLAG_Custom_1;
	}


	return Result;
}

FNetworkPredictionData_Client_Character_TAC::FNetworkPredictionData_Client_Character_TAC(const UCharacterMovementComponent& ClientMovement)
	: Super(ClientMovement)
{
	
}

void UTACCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);
	
	bWantsToSprint = ((Flags & FSavedMove_Character::FLAG_Custom_0) != 0);
	bWantsToProne = ((Flags & FSavedMove_Character::FLAG_Custom_1) != 0);
}

FSavedMovePtr FNetworkPredictionData_Client_Character_TAC::AllocateNewMove()
{
	return MakeShared<FSavedMove_Character_TAC>();
}

FNetworkPredictionData_Client* UTACCharacterMovementComponent::GetPredictionData_Client() const
{
	if (ClientPredictionData == nullptr)
	{
		UTACCharacterMovementComponent* MutableThis = const_cast<UTACCharacterMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_Character_TAC(*this);
	}

	return ClientPredictionData;
}
