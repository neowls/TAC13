// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/TACCharacterBase.h"
#include "InputActionValue.h"
#include "TACCharacterPlayer.generated.h"

/**
 * 
 */
UCLASS()
class TAC13_API ATACCharacterPlayer : public ATACCharacterBase
{
	GENERATED_BODY()
	
public:
	ATACCharacterPlayer(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	
public:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void FireHitCheck() override;
	void FireHitConfirm(AActor* HitActor);
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	void PlayFireAnimation();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCFire(float FireStartTime);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCFire();

	UFUNCTION(Client, Unreliable)
	void ClientRPCPlayerAnimation(ATACCharacterPlayer* CharacterToPlay);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCNotifyHit(const FHitResult& HitResult, float HitCheckTime);

	UPROPERTY(ReplicatedUsing = OnRep_CanFire)
	uint8 bCanFire : 1;

	UFUNCTION()
	void OnRep_CanFire();

	float FireTime = 0.1f;
	float LastFireStartTime = 0.0f;
	float FireTimeDifference = 0.0f;
	float AcceptMinCheckTime = 0.1f;

public:
	void ResetFire();
	
	FTimerHandle FireTimerHandle;
	
#pragma region Input
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

#pragma region Mesh
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character,meta=(AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> ArmMesh;

	UPROPERTY(BlueprintReadOnly, Category = Anim)
	TObjectPtr<class UTACAnimInstance> ArmAnimInstance;
	
	UPROPERTY(BlueprintReadOnly, Category = Anim)
	TObjectPtr<class UTACAnimInstance> BodyAnimInstance;

#pragma endregion
	
};