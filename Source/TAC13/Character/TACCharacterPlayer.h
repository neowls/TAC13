// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/TACCharacterBase.h"
#include "InputActionValue.h"
#include "Weapon/TACWeapon.h"
#include "TACCharacterPlayer.generated.h"

class UTACAnimInstance;
/**
 * 
 */
UCLASS()
class TAC13_API ATACCharacterPlayer : public ATACCharacterBase
{
	GENERATED_BODY()
	
public:
	ATACCharacterPlayer(const FObjectInitializer& ObjectInitializer);

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;
	
	virtual void PossessedBy(AController* NewController) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

#pragma region FIRE

public:
	UFUNCTION()
	void ResetFire();

protected:
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCFire(float FireStartTime);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCFire();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCNotifyHit(const FHitResult& HitResult, float HitCheckTime);

	UFUNCTION()
	void OnRep_CanFire();

	void PlayFireAnimation();
	
	void FireHitConfirm(const FHitResult& HitResult);
	
	virtual void FireHitCheck() override;

	UPROPERTY(ReplicatedUsing = OnRep_CanFire)
	uint8 bCanFire : 1;
	
	FTimerHandle FireTimerHandle;
	
	float FireTime = 0.1f;
	
	float LastFireStartTime = 0.0f;
	
	float FireTimeDifference = 0.0f;
	
	float AcceptMinCheckTime = 0.1f;

#pragma endregion 

#pragma region WEAPON

public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, ReplicatedUsing = OnRep_OwnWeapons, Category = Weapon)
	TArray<TObjectPtr<ATACWeapon>> OwnWeapons;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, ReplicatedUsing = OnRep_CurrentWeapon, Category = Weapon)
	TObjectPtr<ATACWeapon> CurrentWeapon;

	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	TSubclassOf<ATACWeapon> WeaponToSpawn;
	
	UFUNCTION(BlueprintPure)
	FORCEINLINE FTransform GetLeftHandTransform() const { return CurrentWeapon->GetMesh()->GetSocketTransform(FName("S_Left_Hand")); }
	
	UFUNCTION(BlueprintCallable, Category = Weapon)
	void SpawnWeapon(FName WeaponName);

protected:
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite , Category = Weapon)
	uint8 CurrentWeaponIndex;

	uint8 LocalWeaponIndex;

	UFUNCTION(BlueprintCallable, Category = Weapon)
	void EquipWeapon(const uint8 Index);

	UFUNCTION(BlueprintCallable, Category = Weapon)
	void ChangeWeapon(const FInputActionValue& Value);

	UFUNCTION(Server, Reliable)
	void ServerRPCSetCurrentWeapon(const uint8 Index);
	
	UFUNCTION()
	void OnRep_CurrentWeapon();

	UFUNCTION()
	void OnRep_OwnWeapons();

	UFUNCTION()
	void AttachWeapon(ATACWeapon* TargetWeapon);
	
	void PlayChangeWeaponAnimation();
	
	virtual void ChangeWeaponCheck() override;
	
#pragma endregion 
	
#pragma region INPUT
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> AimAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> CrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> SneakAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ProneAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> MeleeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ChangeFireModeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> LeaningAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ChangeWeaponAction;


	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Fire(const FInputActionValue& Value);
	void Aim(const FInputActionValue& Value);
	void Sneak(const FInputActionValue& Value);
	void TryCrouch();
	void TrySprint(const FInputActionValue& Value);
	void Melee();
	void ChangeFireMode();
	void Leaning(const FInputActionValue& Value);

	void SetCharacterControl();
	virtual void SetCharacterControlData(const UTACControlData* CharacterControlData) override;
#pragma endregion

#pragma region MESH
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character,meta=(AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> ArmMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character,meta=(AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> RoleMesh;

	UPROPERTY(BlueprintReadOnly, Category = Anim)
	TObjectPtr<UTACAnimInstance> ArmAnimInstance;
	
	UPROPERTY(BlueprintReadOnly, Category = Anim)
	TObjectPtr<UTACAnimInstance> BodyAnimInstance;

	UFUNCTION(BlueprintPure)
	FORCEINLINE USkeletalMeshComponent* GetArmMesh() const { return ArmMesh; }



#pragma endregion
	
};