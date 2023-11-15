// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/TACAnimationFireInterface.h"
#include "TACCharacterBase.generated.h"

UCLASS()
class TAC13_API ATACCharacterBase : public ACharacter, public ITACAnimationFireInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATACCharacterBase(const FObjectInitializer& ObjectInitializer);

	/** Returns CameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetCameraComponent() const { return Camera; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE class UTACCharacterMovementComponent* GetTACCharacterMovement() const { return TACCharacterMovement; }
	
	virtual void PostInitializeComponents() override;
	
protected:
	/** Movement component used for movement logic in various movement modes (walking, falling, etc), containing relevant settings and functions to control movement. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Character, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTACCharacterMovementComponent> TACCharacterMovement;
	
	virtual void SetCharacterControlData(const class UTACControlData* CharacterControlData);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(VisibleAnywhere, Category = CharacterControl, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTACControlData> CurrentControlData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(BlueprintReadOnly, Category = Camera)
	float CameraStandHeight;

	UPROPERTY(BlueprintReadOnly, Category = Camera)
	float CameraCrouchedHeight;
	
	UPROPERTY(BlueprintReadOnly, Category = Camera)
	float CameraProneHeight;
	
public:
	UPROPERTY(BlueprintReadOnly, Category = Aiming)
	uint8 bIsADS : 1;
	
//	Crouch Section
public:
	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

//	Montage Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> FireArmMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> FireCosmeticMontage;

//	Hit Section
protected:
	virtual void FireHitCheck() override;

//	Prone Section
#pragma region Prone
	private:
	friend class FSavedMove_Character_TAC;

public:
	/** Default proned eye height */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Camera)
	float PronedEyeHeight;
	
public:
	/** Set by character movement to specify that this Character is currently Proned. */
	UPROPERTY(BlueprintReadOnly, replicatedUsing=OnRep_IsProned, Category=Character)
	uint32 bIsProned:1;

public:
	virtual void RecalculateBaseEyeHeight() override;

	/** Handle Crouching replicated from server */
	UFUNCTION()
	virtual void OnRep_IsProned();

	/**
	 * Request the character to start Proned. The request is processed on the next update of the CharacterMovementComponent.
	 * @see OnStartProne
	 * @see IsProned
	 * @see CharacterMovement->WantsToProne
	 */
	UFUNCTION(BlueprintCallable, Category=Character, meta=(HidePin="bClientSimulation"))
	virtual void Prone(bool bClientSimulation = false);

	/**
	 * Request the character to stop Proned. The request is processed on the next update of the CharacterMovementComponent.
	 * @see OnEndProne
	 * @see IsProned
	 * @see CharacterMovement->WantsToProne
	 */
	UFUNCTION(BlueprintCallable, Category=Character, meta=(HidePin="bClientSimulation"))
	virtual void UnProne(bool bClientSimulation = false);

	/** @return true if this character is currently able to Prone (and is not currently Proned) */
	UFUNCTION(BlueprintCallable, Category=Character)
	virtual bool CanProne() const;
	
	/** Called when Character stops Proned. Called on non-owned Characters through bIsProned replication. */
	virtual void OnEndProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust);

	/** Event when Character stops Proned. */
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnEndProne", ScriptName="OnEndProne"))
	void K2_OnEndProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust);

	/** Called when Character Pronees. Called on non-owned Characters through bIsProned replication. */
	virtual void OnStartProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust);

	/** Event when Character Pronees. */
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnStartProne", ScriptName="OnStartProne"))
	void K2_OnStartProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust);

#pragma endregion

//	Sprint Section	
#pragma region Sprint
// Sprint Section
public:
	/** Set by character movement to specify that this Character is currently Sprinting. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, replicatedUsing=OnRep_IsSprinting, Category=Character)
	uint32 bIsSprinting:1;

public:
	/** Handle Crouching replicated from server */
	UFUNCTION()
	virtual void OnRep_IsSprinting();

	/**
	 * Request the character to start Sprinting. The request is processed on the next update of the CharacterMovementComponent.
	 * @see OnStartSprint
	 * @see IsSprinting
	 * @see CharacterMovement->WantsToSprint
	 */
	UFUNCTION(BlueprintCallable, Category=Character, meta=(HidePin="bClientSimulation"))
	virtual void Sprint(bool bClientSimulation = false);

	/**
	 * Request the character to stop Sprinting. The request is processed on the next update of the CharacterMovementComponent.
	 * @see OnEndSprint
	 * @see IsSprinting
	 * @see CharacterMovement->WantsToSprint
	 */
	UFUNCTION(BlueprintCallable, Category=Character, meta=(HidePin="bClientSimulation"))
	virtual void UnSprint(bool bClientSimulation = false);

	/** Called when Character stops Sprinting. Called on non-owned Characters through bIsSprinting replication. */
	virtual void OnEndSprint();

	/** Event when Character stops Sprinting. */
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnEndSprint", ScriptName="OnEndSprint"))
	void K2_OnEndSprint();

	/** Called when Character Sprintes. Called on non-owned Characters through bIsSprinting replication. */
	virtual void OnStartSprint();

	/** Event when Character Sprintes. */
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnStartSprint", ScriptName="OnStartSprint"))
	void K2_OnStartSprint();

#pragma endregion 
};
