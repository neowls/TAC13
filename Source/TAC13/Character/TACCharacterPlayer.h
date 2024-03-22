// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/TACCharacterBase.h"
#include "InputActionValue.h"
#include "Interface/TACCharacterHUDInterface.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "TACCharacterPlayer.generated.h"

class ATACPlayerState;
class UTACAnimInstance;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnScoreboardChangedDelegate, uint8 /*bIsScoreboardOn*/);



UCLASS()
class TAC13_API ATACCharacterPlayer : public ATACCharacterBase, public ITACCharacterHUDInterface
{
	GENERATED_BODY()
	
public:
	ATACCharacterPlayer(const FObjectInitializer& ObjectInitializer);

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
	virtual void PostInitializeComponents() override;
	
	virtual void BeginPlay() override;

	virtual void Destroyed() override;
	
	virtual void PossessedBy(AController* NewController) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void SetDead() override;

	virtual void RespawnCharacter() override;
	
	FTimerHandle RespawnTimer;

	float RespawnDelayTime = 3.0f;

	FOnScoreboardChangedDelegate OnScoreboardChanged;

	IOnlineSessionPtr OnlineSessionInterface;

#pragma region FIRE

public:
	UFUNCTION()
	void ResetFire();

protected:
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCFire(float FireStartTime);

	UFUNCTION()
	bool IsCanFire() const;
	
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCFire();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCNotifyHit(const FHitResult& HitResult, float HitCheckTime);
	
	virtual void FireHitCheck() override;

	void SetRecoilPoint();
	
	
	FTimerHandle FireTimerHandle;
	
	float LastFireStartTime = 0.0f;
	
	float FireTimeDifference = 0.0f;
	
	float AcceptMinCheckTime = 0.1f;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ReloadAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ScoreboardAction;

	
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Fire(const FInputActionValue& Value);
	void Aim(const FInputActionValue& Value);
	void Sneak(const FInputActionValue& Value);
	void TryCrouch();
	void TrySprint(const FInputActionValue& Value);
	void Melee();
	void TryChangeWeapon(const FInputActionValue& Value);
	virtual void ChangeFireMode() override;
	void Leaning(const FInputActionValue& Value);
	void ShowScoreboard(const FInputActionValue& Value);

	void SetCharacterControl(TObjectPtr<UTACControlData> InCharacterControlData);
	virtual void SetCharacterControlData(const UTACControlData* CharacterControlData) override;
#pragma endregion

protected:
	virtual void SetupHUDWidget(class UTACHUDWidget* InHUDWidget) override;

};