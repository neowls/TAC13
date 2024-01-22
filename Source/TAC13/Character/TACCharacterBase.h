// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/TACAnimationWeaponInterface.h"
#include "Interface/TACCharacterWidgetInterface.h"
#include "TACCharacterBase.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCurrentAmmoChangedDelegate, uint8 /*CurrentAmmo*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnOwnAmmoChangedDelegate, uint8 /*OwnAmmo*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnWeaponNameChangedDelegate, FName /*WeaponName*/);


UCLASS()
class TAC13_API ATACCharacterBase : public ACharacter, public ITACAnimationWeaponInterface, public ITACCharacterWidgetInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATACCharacterBase(const FObjectInitializer& ObjectInitializer);

	
	FOnCurrentAmmoChangedDelegate OnCurrentAmmoChanged;
	FOnOwnAmmoChangedDelegate OnOwnAmmoChanged;
	FOnWeaponNameChangedDelegate OnWeaponNameChanged;

	/** Returns CameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetCameraComponent() const { return Camera; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE class UTACCharacterMovementComponent* GetTACCharacterMovement() const { return TACCharacterMovement; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE USkeletalMeshComponent* GetArmMesh() const { return ArmMesh; }


protected:
	/** Movement component used for movement logic in various movement modes (walking, falling, etc), containing relevant settings and functions to control movement. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Character, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTACCharacterMovementComponent> TACCharacterMovement;
	
	virtual void SetCharacterControlData(const class UTACControlData* CharacterControlData);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PostInitializeComponents() override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void SetDead();

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
	UPROPERTY(BlueprintReadOnly, replicatedUsing=OnRep_IsADS, Category = Aiming)
	uint8 bIsADS : 1;

public:
	UFUNCTION()
	virtual void OnRep_IsADS();
	virtual void OnStartADS();
	virtual void OnEndADS();
	virtual void ChangeWeaponCheck() override;

	void PlayDeadAnimation();
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> FireMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> ChangeWeaponMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> DeadMontage;

//	Hit Section
protected:
	virtual void FireHitCheck() override;

private:
	friend class FSavedMove_Character_TAC;
	
#pragma region Sprint
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



#pragma region MESH
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character,meta=(AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> ArmMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character,meta=(AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> RoleMesh;

	UPROPERTY(BlueprintReadOnly, Category = Anim)
	TObjectPtr<class UTACArmAnimInstance> ArmAnimInstance;
	
	UPROPERTY(BlueprintReadOnly, Category = Anim)
	TObjectPtr<class UTACBodyAnimInstance> BodyAnimInstance;

#pragma endregion

//	Stat Section
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UTACCharacterStatComponent> Stat;

//	UI Widget Section
protected:
	virtual void SetupCharacterWidget(UTACUserWidget* InUserWidget) override;
};
