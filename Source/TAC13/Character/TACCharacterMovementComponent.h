// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TACCharacterMovementComponent.generated.h"

class ATACCharacterBase;

/**
 * 
 */
UCLASS()
class TAC13_API UTACCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	UTACCharacterMovementComponent(const FObjectInitializer& ObjectInitializer);

public:
	UPROPERTY(Category="Character Movement (General Settings)", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0", ForceUnits="cm/s"))
	float MaxBaseSpeed;

	UFUNCTION(BlueprintCallable)
	const float GetGroundDistance();
	
protected:
	virtual void UnCrouch(bool bClientSimulation) override;

public:
	virtual bool HasValidData() const override;
	virtual void PostLoad() override;
	virtual void SetUpdatedComponent(USceneComponent* NewUpdatedComponent) override;

public:
	virtual float GetMaxAcceleration() const override;
	virtual float GetMaxSpeed() const override;
	virtual float GetMaxBrakingDeceleration() const override;
	
	virtual void CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration) override;
	virtual void ApplyVelocityBraking(float DeltaTime, float Friction, float BrakingDeceleration) override;

	virtual bool CanWalkOffLedges() const override;
	virtual bool CanAttemptJump() const override;
	
	
#pragma region Prone
	private:
	/** Character movement component belongs to */
	UPROPERTY(Transient, DuplicateTransient)
	TObjectPtr<ATACCharacterBase> TACCharacterOwner;

public:
	/** Max Acceleration (rate of change of velocity) */
	UPROPERTY(Category="Character Movement (General Settings)", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0"))
	float MaxAccelerationProned;
	
	/** The maximum ground speed when Proned. */
	UPROPERTY(Category="Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0", ForceUnits="cm/s"))
	float MaxWalkSpeedProned;

	/**
	 * Deceleration when walking and not applying acceleration. This is a constant opposing force that directly lowers velocity by a constant value.
	 * @see GroundFriction, MaxAcceleration
	 */
	UPROPERTY(Category="Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0"))
	float BrakingDecelerationProned;

	/**
	 * Setting that affects movement control. Higher values allow faster changes in direction.
	 * If bUseSeparateBrakingFriction is false, also affects the ability to stop more quickly when braking (whenever Acceleration is zero), where it is multiplied by BrakingFrictionFactor.
	 * When braking, this property allows you to control how much friction is applied when moving across the ground, applying an opposing force that scales with current velocity.
	 * This can be used to simulate slippery surfaces such as ice or oil by changing the value (possibly based on the material pawn is standing on).
	 * @see BrakingDecelerationWalking, BrakingFriction, bUseSeparateBrakingFriction, BrakingFrictionFactor
	 */
	UPROPERTY(Category="Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0"))
	float GroundFrictionProned;

	/**
	 * Friction (drag) coefficient applied when braking (whenever Acceleration = 0, or if character is exceeding max speed); actual value used is this multiplied by BrakingFrictionFactor.
	 * When braking, this property allows you to control how much friction is applied when moving across the ground, applying an opposing force that scales with current velocity.
	 * Braking is composed of friction (velocity-dependent drag) and constant deceleration.
	 * This is the current value, used in all movement modes; if this is not desired, override it or bUseSeparateBrakingFriction when movement mode changes.
	 * @note Only used if bUseSeparateBrakingFriction setting is true, otherwise current friction such as GroundFriction is used.
	 * @see bUseSeparateBrakingFriction, BrakingFrictionFactor, GroundFriction, BrakingDecelerationWalking
	 */
	UPROPERTY(Category="Character Movement (General Settings)", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0", EditCondition="bUseSeparateBrakingFriction"))
	float BrakingFrictionProned;
	
	/** Collision half-height when proned (component scale is applied separately) */
	UPROPERTY(Category="Character Movement (General Settings)", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0", ForceUnits=cm))
	float PronedHalfHeight;

	/** Collision half-height when proned (component scale is applied separately) */
	UPROPERTY(Category="Character Movement (General Settings)", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0", ForceUnits=cm))
	float PronedRadius;

	/**
	 * Cannot leave prone for this duration
	 * @see ClearProneLock
	 */
	UPROPERTY(Category="Character Movement (General Settings)", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0", ForceUnits=cm))
	float ProneLockDuration;
	
	/** If true, Character can walk off a ledge when proned. */
	UPROPERTY(Category="Character Movement: Walking", EditAnywhere, BlueprintReadWrite)
	uint8 bCanWalkOffLedgesWhenProned:1;
	
public:
	/** If true, try to Prone (or keep Proned) on next update. If false, try to stop Proned on next update. */
	UPROPERTY(Category="Character Movement (General Settings)", VisibleInstanceOnly, BlueprintReadOnly)
	uint8 bWantsToProne:1;

	UPROPERTY(Category="Character Movement (General Settings)", VisibleInstanceOnly, BlueprintReadOnly)
	uint8 bProneLocked:1;

protected:
	float ProneLockTimestamp = -1.f;
	
public:
	UFUNCTION(BlueprintPure, Category="Character Movement")
	bool IsProneLocked() const { return bProneLocked; }
	
	bool IsProneLockOnTimer() const { return GetRemainingProneLockCooldown() > 0.f; }

	// Prone lock timer
	float GetRemainingProneLockCooldown() const
	{
		const float CurrentTimestamp = GetTimestamp();
		const float RemainingCooldown = ProneLockDuration - (CurrentTimestamp - ProneLockTimestamp);
		return FMath::Clamp(RemainingCooldown, 0.f, ProneLockDuration);
	}

	void SetProneLock(bool bLock)
	{
		if (bLock)
		{
			bProneLocked = true;
			ProneLockTimestamp = GetTimestamp();
		}
		else
		{
			bProneLocked = false;
		}
	}

	float GetTimestamp() const;

public:
	virtual bool IsProned() const;

	/**
	 * Call CharacterOwner->OnStartProne() if successful.
	 * In general you should set bWantsToProne instead to have the Prone persist during movement, or just use the Prone functions on the owning Character.
	 * @param	bClientSimulation	true when called when bIsProned is replicated to non owned clients.
	 */
	virtual void Prone(bool bClientSimulation = false);
	
	/**
	 * Checks if default capsule size fits (no encroachment), and trigger OnEndProne() on the owner if successful.
	 * @param	bClientSimulation	true when called when bIsProned is replicated to non owned clients.
	 */
	virtual void UnProne(bool bClientSimulation = false);

	/** Returns true if the character is allowed to Prone in the current state. By default it is allowed when walking or falling. */
	virtual bool CanProneInCurrentState() const;

	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
	virtual void UpdateCharacterStateAfterMovement(float DeltaSeconds) override;

protected:
	virtual bool ClientUpdatePositionAfterServerUpdate() override;

	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	
public:
	/** Get prediction data for a client game. Should not be used if not running as a client. Allocates the data on demand and can be overridden to allocate a custom override if desired. Result must be a FNetworkPredictionData_Client_Character. */
	virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;
#pragma endregion
#pragma region Sprint
	public:
	/** Max Acceleration (rate of change of velocity) */
	UPROPERTY(Category="Character Movement (General Settings)", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0"))
	float MaxAccelerationSprinting;
	
	/** The maximum ground speed when Sprinting. */
	UPROPERTY(Category="Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0", ForceUnits="cm/s"))
	float MaxWalkSpeedSprinting;

	/**
	 * Deceleration when walking and not applying acceleration. This is a constant opposing force that directly lowers velocity by a constant value.
	 * @see GroundFriction, MaxAcceleration
	 */
	UPROPERTY(Category="Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0"))
	float BrakingDecelerationSprinting;

	/**
	 * Setting that affects movement control. Higher values allow faster changes in direction.
	 * If bUseSeparateBrakingFriction is false, also affects the ability to stop more quickly when braking (whenever Acceleration is zero), where it is multiplied by BrakingFrictionFactor.
	 * When braking, this property allows you to control how much friction is applied when moving across the ground, applying an opposing force that scales with current velocity.
	 * This can be used to simulate slippery surfaces such as ice or oil by changing the value (possibly based on the material pawn is standing on).
	 * @see BrakingDecelerationWalking, BrakingFriction, bUseSeparateBrakingFriction, BrakingFrictionFactor
	 */
	UPROPERTY(Category="Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0"))
	float GroundFrictionSprinting;

	/**
     * When struggling to surpass walk speed, which can occur with heavy rotation and low acceleration, we
     * mitigate the check so there isn't a constant re-entry that can occur as an edge case.
     * This can optionally be used inversely, to require you to considerably exceed MaxSpeedWalking before sprinting
     * will actually take effect.
     */
    UPROPERTY(Category="Character Movement: Walking", AdvancedDisplay, EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0"))
    float VelocityCheckMitigatorSprinting;
	
	/**
	 * Friction (drag) coefficient applied when braking (whenever Acceleration = 0, or if character is exceeding max speed); actual value used is this multiplied by BrakingFrictionFactor.
	 * When braking, this property allows you to control how much friction is applied when moving across the ground, applying an opposing force that scales with current velocity.
	 * Braking is composed of friction (velocity-dependent drag) and constant deceleration.
	 * This is the current value, used in all movement modes; if this is not desired, override it or bUseSeparateBrakingFriction when movement mode changes.
	 * @note Only used if bUseSeparateBrakingFriction setting is true, otherwise current friction such as GroundFriction is used.
	 * @see bUseSeparateBrakingFriction, BrakingFrictionFactor, GroundFriction, BrakingDecelerationWalking
	 */
	UPROPERTY(Category="Character Movement (General Settings)", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0", EditCondition="bUseSeparateBrakingFriction"))
	float BrakingFrictionSprinting;
	
public:
	/** If true, try to Sprint (or keep Sprinting) on next update. If false, try to stop Sprinting on next update. */
	UPROPERTY(Category="Character Movement (General Settings)", VisibleInstanceOnly, BlueprintReadOnly)
	uint8 bWantsToSprint:1;

public:
	virtual bool IsSprintingAtSpeed() const;
	
public:
	virtual bool IsSprinting() const;

	/**
	 * Call CharacterOwner->OnStartSprint() if successful.
	 * In general you should set bWantsToSprint instead to have the Sprint persist during movement, or just use the Sprint functions on the owning Character.
	 * @param	bClientSimulation	true when called when bIsSprinted is replicated to non owned clients.
	 */
	virtual void Sprint(bool bClientSimulation = false);
	
	/**
	 * Checks if default capsule size fits (no encroachment), and trigger OnEndSprint() on the owner if successful.
	 * @param	bClientSimulation	true when called when bIsSprinted is replicated to non owned clients.
	 */
	virtual void UnSprint(bool bClientSimulation = false);

	/** Returns true if the character is allowed to Sprint in the current state. */
	virtual bool CanSprintInCurrentState() const;

	/**
	 * This check ensures that we are not sprinting backward or sideways, while allowing leeway 
	 * This angle allows sprinting when holding forward, forward left, forward right
	 * but not left or right or backward)
	 * 
	 * You can override this to remove this check, or to add your own check. Magic numbers are used to avoid
	 * more expensive runtime trig calculations.
	 *
	 * Consider adding this check to CanSprintInCurrentState() if you want the check to cause Sprint to end
	 * when it fails while already sprinting
	 */
	virtual bool IsSprintWithinAllowableInputAngle() const;
#pragma endregion 
};

class TAC13_API FSavedMove_Character_TAC : public FSavedMove_Character
{
	using Super = FSavedMove_Character;

public:
	FSavedMove_Character_TAC()
		: bWantsToProne(0)
		, bProneLocked(0)
		, bWantsToSprint(0)
	{
	}

	virtual ~FSavedMove_Character_TAC() override
	{}

	uint32 bWantsToProne:1;
	uint32 bProneLocked:1;
	uint32 bWantsToSprint:1;
		
	/** Clear saved move properties, so it can be re-used. */
	virtual void Clear() override;

	/** Called to set up this saved move (when initially created) to make a predictive correction. */
	virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character & ClientData) override;
	virtual void PrepMoveFor(ACharacter* C) override;

	/** Returns a byte containing encoded special movement information (jumping, crouching, etc.)	 */
	virtual uint8 GetCompressedFlags() const override;
};

class TAC13_API FNetworkPredictionData_Client_Character_TAC : public FNetworkPredictionData_Client_Character
{
	using Super = FNetworkPredictionData_Client_Character;

public:
	FNetworkPredictionData_Client_Character_TAC(const UCharacterMovementComponent& ClientMovement);

	virtual FSavedMovePtr AllocateNewMove() override;
};