#pragma once

#include "CoreMinimal.h"
#include "Game/TACPlayPlayerState.h"
#include "GameFramework/Character.h"
#include "Interface/TACAnimationWeaponInterface.h"
#include "Interface/TACCharacterWidgetInterface.h"
#include "Weapon/TACWeapon.h"
#include "TACCharacterBase.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCurrentAmmoChangedDelegate, uint8 /*CurrentAmmo*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnOwnAmmoChangedDelegate, uint8 /*OwnAmmo*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnWeaponNameChangedDelegate, FString /*WeaponName*/);

template<typename T>
static void InitializeObjectFinder(TObjectPtr<T>& Ref, const FString& Path)
{
	ConstructorHelpers::FObjectFinder<T> Finder(*Path);
	if (Finder.Succeeded()) {
		Ref = Finder.Object;
	}
}

UCLASS()
class TAC13_API ATACCharacterBase : public ACharacter, public ITACAnimationWeaponInterface, public ITACCharacterWidgetInterface
{
	GENERATED_BODY()

public:
	ATACCharacterBase(const FObjectInitializer& ObjectInitializer);

	// Get Section
	FORCEINLINE class UCameraComponent* GetCameraComponent() const { return FirstCamera; }
    
    UFUNCTION(BlueprintCallable)
    FORCEINLINE class UTACCharacterMovementComponent* GetTACCharacterMovement() const { return TACCharacterMovement; }
   
    UFUNCTION(BlueprintPure)
    FORCEINLINE USkeletalMeshComponent* GetArmMesh() const { return ArmMesh; }

	
	// Delegate Section
	FOnCurrentAmmoChangedDelegate OnCurrentAmmoChanged;		// Current Ammo
	FOnOwnAmmoChangedDelegate OnOwnAmmoChanged;				// Own Ammo
	FOnWeaponNameChangedDelegate OnWeaponNameChanged;		// Weapon Name


	UPROPERTY(BlueprintReadOnly, replicatedUsing=OnRep_IsADS, Category = Aiming)
	uint8 bIsADS : 1;
	
	// ADS Section
	UFUNCTION()
	virtual void OnRep_IsADS();

	UFUNCTION()
	virtual void OnStartADS();

	UFUNCTION()
	virtual void OnEndADS();
	

	//	Crouch Section

	FTimerHandle CrouchTimerHandle;
	
	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;


protected:
	// General Function
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PostInitializeComponents() override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void Tick(float DeltaSeconds) override;

	
	virtual void SetDead();

	virtual void RespawnCharacter();
	
	virtual void FireHitCheck() override;
	
	virtual void SetupCharacterWidget(UTACUserWidget* InUserWidget) override;
	
	
	// Control Section
    virtual void SetCharacterControlData(const class UTACControlData* CharacterControlData);
    
    UPROPERTY(VisibleAnywhere, Category = CharacterControl, Meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UTACControlData> CurrentControlData;
    
    UPROPERTY(VisibleAnywhere, Category = CharacterControl, Meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UTACControlData> PlayControlData;
    
    UPROPERTY(VisibleAnywhere, Category = CharacterControl, Meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UTACControlData> SpectateControlData;

	
	// Camera Section
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
    TObjectPtr<UCameraComponent> FirstCamera;
    
    UPROPERTY(BlueprintReadOnly, Category = Camera)
    float CameraStandHeight;
   
    UPROPERTY(BlueprintReadOnly, Category = Camera)
    float CameraCrouchedHeight;
    
    UPROPERTY(BlueprintReadOnly, Category = Camera)
    float CameraProneHeight;

	UPROPERTY()
	float TargetCameraHeight;

	UFUNCTION()
	void UpdateCameraHeight();
	

	//	Stat Section
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UTACCharacterStatComponent> Stat;
    

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Character, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTACCharacterMovementComponent> TACCharacterMovement;

	UPROPERTY()
	ATACPlayPlayerState* RecentAttacker;

	UPROPERTY()
	FString RecentAttackedWeaponName;

	void HitConfirm(const FHitResult& HitResult);


#pragma region WEAPON

public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, ReplicatedUsing = OnRep_OwnWeapons, Category = Weapon)
	TArray<TObjectPtr<ATACWeapon>> OwnWeapons;

	UPROPERTY(VisibleInstanceOnly, Category = Weapon)
	TObjectPtr<ATACWeapon> CurrentWeapon;

	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	TSubclassOf<ATACWeapon> WeaponToSpawn;

	TObjectPtr<UStaticMeshComponent> GetCurrentSight() const { return CurrentWeapon ? CurrentWeapon->GetSightMesh() : nullptr; }
	
	UFUNCTION(BlueprintPure)
	FORCEINLINE FTransform GetLeftHandTransform() const { return CurrentWeapon->GetMesh()->GetSocketTransform(FName("S_Left_Hand")); }
	
	UFUNCTION(BlueprintCallable, Category = Weapon)
	void SpawnWeapon(FName WeaponName);

	UFUNCTION()
	void ReloadingWeapon();

protected:
	
	UPROPERTY(VisibleInstanceOnly, ReplicatedUsing = OnRep_CurrentWeaponIndex , Category = Weapon)
	uint8 CurrentWeaponIndex;
	
	UFUNCTION()
	void ChangeWeapon(const int8& ChangeValue);

	UFUNCTION(Server, Reliable)
	void ServerChangeWeapon(const uint8 Index);

	UFUNCTION(Server, Reliable)
	void ServerReloadWeapon();
	
	UFUNCTION()
	void OnRep_OwnWeapons();

	UFUNCTION()
	void OnRep_CurrentWeaponIndex();
	
	UFUNCTION()
	void AttachWeapon(ATACWeapon* TargetWeapon);

	UFUNCTION()
	void DropWeapon();

	UFUNCTION()
	void ApplyWeaponChange(const uint8 NewWeaponIndex);

	UFUNCTION()
	void CurrentWeaponCosmetic();

	UFUNCTION()
	virtual void ChangeWeaponCheck() override;

	//Fire Section
	
	float FireTime = 0.1f;
	
	UPROPERTY(ReplicatedUsing = OnRep_CanFire)
	uint8 bCanFire : 1;

	UFUNCTION()
	virtual void ChangeFireMode();

	UFUNCTION()
	void OnRep_CanFire();

#pragma endregion 
	
#pragma region Sprint
public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, replicatedUsing=OnRep_IsSprinting, Category=Character)
	uint32 bIsSprinting:1;

public:
	UFUNCTION()
	virtual void OnRep_IsSprinting();
	
	UFUNCTION(BlueprintCallable, Category=Character)
	virtual void Sprint();
	
	UFUNCTION(BlueprintCallable, Category=Character)
	virtual void UnSprint();

	/** Called when Character stops Sprinting. Called on non-owned Characters through bIsSprinting replication. */
	virtual void OnEndSprint();
	
	/** Called when Character Sprinting. Called on non-owned Characters through bIsSprinting replication. */
	virtual void OnStartSprint();

#pragma endregion

#pragma region Mesh
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character,meta=(AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> ArmMesh;
	
	UPROPERTY(BlueprintReadOnly, Category = Anim)
	TObjectPtr<class UTACArmAnimInstance> ArmAnimInstance;
	
	UPROPERTY(BlueprintReadOnly, Category = Anim)
	TObjectPtr<class UTACBodyAnimInstance> BodyAnimInstance;

#pragma endregion

#pragma region Montage
	
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
	TObjectPtr<class UAnimMontage> ChangeWeaponArmMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> ChangeWeaponCosmeticMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> DeadMontage;
	
	void PlayMontageAnimation(const TObjectPtr<UAnimMontage>& MontageToPlay);
	
#pragma endregion

	
private:
	friend class FSavedMove_Character_TAC;

};
