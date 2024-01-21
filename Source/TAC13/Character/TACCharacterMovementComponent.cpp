// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TACCharacterMovementComponent.h"

#include "TAC13.h"
#include "Components/CapsuleComponent.h"
#include "Character/TACCharacterBase.h"
#include "GameFramework/Character.h"


UTACCharacterMovementComponent::UTACCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

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

	if (MovementMode == MOVE_Walking) return 0.0f;
	if (HitResult.bBlockingHit) return FMath::Max((HitResult.Distance - CapsuleHalfHeight), 0.0f);

	return 0.0f;
}

void UTACCharacterMovementComponent::UnCrouch(bool bClientSimulation)
{
	Super::UnCrouch(bClientSimulation);
	//if (CharacterOwner->bIsCrouched) bWantsToCrouch = true;
	TAC_SUBLOG(LogTACNetwork, Log, TEXT("%s"), TEXT("Begin"));
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
		if (IsSprinting()) return MaxAccelerationSprinting;
	}

	return Super::GetMaxAcceleration();
}

float UTACCharacterMovementComponent::GetMaxSpeed() const
{
	if (IsMovingOnGround())
	{
		if (IsSprinting()) return MaxWalkSpeedSprinting;
	}
	return Super::GetMaxSpeed();
}

float UTACCharacterMovementComponent::GetMaxBrakingDeceleration() const
{
	if (IsMovingOnGround())
	{
		if (IsSprinting()) return BrakingDecelerationSprinting;
	}
	return Super::GetMaxBrakingDeceleration();
}

void UTACCharacterMovementComponent::CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration)
{
	if (IsMovingOnGround())
	{
		if (IsSprinting()) Friction = GroundFrictionSprinting;
	}
	Super::CalcVelocity(DeltaTime, Friction, bFluid, BrakingDeceleration);
}

void UTACCharacterMovementComponent::ApplyVelocityBraking(float DeltaTime, float Friction, float BrakingDeceleration)
{
	if (IsMovingOnGround())
	{
		if (IsSprinting()) Friction = (bUseSeparateBrakingFriction ? BrakingFrictionSprinting : GroundFrictionSprinting);
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

	if (IsFalling() && !IsMovingOnGround())
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
	static constexpr float MaxSprintInputNormal = 0.64278732; // cos(rad(MaxSprintInputDegrees))

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


bool UTACCharacterMovementComponent::IsADS() const
{
	return TACCharacterOwner && TACCharacterOwner->bIsADS;
}

bool UTACCharacterMovementComponent::CanADS() const
{
	if (!UpdatedComponent || UpdatedComponent->IsSimulatingPhysics())
	{
		return false;
	}

	if (IsFalling())
	{
		return false;
	}

	if (!IsMovingOnGround())
	{
		return false;
	}

	if(IsSprinting() && Velocity.Size() > 0)
	{
		return false;
	}

	return true;
}

void UTACCharacterMovementComponent::ADS(bool bClientSimulation)
{
	if (!HasValidData())
	{
		return;
	}

	if (!bClientSimulation)
	{
		TACCharacterOwner->bIsADS = true;
	}
	TACCharacterOwner->OnStartADS();
}

void UTACCharacterMovementComponent::UnADS(bool bClientSimulation)
{
	if (!HasValidData())
	{
		return;
	}

	if (!bClientSimulation)
	{
		TACCharacterOwner->bIsADS = false;
	}
	TACCharacterOwner->OnEndADS();
}

void UTACCharacterMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	// Proxies get replicated crouch state.
	if (CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)
	{
		// Check for a change in crouch state. Players toggle crouch by changing bWantsToCrouch.
		const bool bIsCrouching = IsCrouching();
		if (bIsCrouching && (!bWantsToCrouch || !CanCrouchInCurrentState()))
		{
			UnCrouch(false);
		}
		else if (!bIsCrouching && bWantsToCrouch && CanCrouchInCurrentState())
		{
			Crouch(false);
		}

		
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

		
		// Check for a change in ADS state. Players toggle Sprint by changing bWantsToADS.
		const bool bIsADS = IsADS();
		if (bIsADS && (!bWantsToADS || !CanADS() ))
		{
			UnADS();
		}
		else if (!bIsADS && bWantsToADS && CanADS())
		{
			ADS();
		}
		
	}
}

void UTACCharacterMovementComponent::UpdateCharacterStateAfterMovement(float DeltaSeconds)
{
	//Super::UpdateCharacterStateAfterMovement(DeltaSeconds);

	if (CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)
	{
		// UnSprint if no longer allowed to be Sprinting
		if (IsSprinting() && !CanSprintInCurrentState())
		{
			UnSprint(false);
		}
		
		if(IsADS() && !CanADS())
		{
			UnADS(false);
		}
	}
}

bool UTACCharacterMovementComponent::ClientUpdatePositionAfterServerUpdate()
{
	const bool bRealSprint = bWantsToSprint;
	const bool bRealADS = bWantsToADS;
	const bool bResult = Super::ClientUpdatePositionAfterServerUpdate();
	bWantsToSprint = bRealSprint;
	bWantsToADS = bRealADS;

	return bResult;
}

void FSavedMove_Character_TAC::Clear()
{
	Super::Clear();

	bWantsToSprint = false;
	bWantsToADS = false;
}

void FSavedMove_Character_TAC::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel,
                                          FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);
	bWantsToSprint = Cast<ATACCharacterBase>(C)->GetTACCharacterMovement()->bWantsToSprint;
	bWantsToADS = Cast<ATACCharacterBase>(C)->GetTACCharacterMovement()->bWantsToADS;
}


uint8 FSavedMove_Character_TAC::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();

	if (bWantsToSprint)
	{
		Result |= FLAG_Custom_0;
	}

	if (bWantsToADS)
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
	bWantsToADS = ((Flags & FSavedMove_Character::FLAG_Custom_1) != 0);
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
