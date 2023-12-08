// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "TACWeapon.generated.h"

class ATACCharacterPlayer;

UENUM(BlueprintType)
enum class EFireMode:uint8
{
	SINGLE,
	BURST,
	FULL
};

USTRUCT()
struct FTACWeaponStat : public FTableRowBase
{
	GENERATED_BODY()

public:
	FTACWeaponStat() : Damage(0.0f), FireRate(0.0f), Recoil(0.0f), Ergonomics(0.0f), MaxAmmo(0) {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
	TObjectPtr<class USkeletalMesh> OwnMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
	TArray<EFireMode> OwnFireMode;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
	float FireRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
	float Recoil;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
	float Ergonomics;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
	uint8 MaxAmmo;
};


UCLASS()
class TAC13_API ATACWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	ATACWeapon();

	FORCEINLINE FTACWeaponStat GetWeaponStat() const { return WeaponStat; }
	void SetWeaponStat(const FTACWeaponStat& InWeaponStat) { WeaponStat = InWeaponStat; }

	virtual void SetOwner(AActor* NewOwner) override;

	void ChangeFireMode();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	TObjectPtr<USceneComponent> Root;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Weapon)
	TObjectPtr<USkeletalMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	TObjectPtr<UStaticMeshComponent> Sight;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = Weapon)
	TObjectPtr<ATACCharacterPlayer> CurrentOwner;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_SetWeaponStatData , Category = Stat)
	FTACWeaponStat WeaponStat;

	UPROPERTY()
	EFireMode CurrentFireMode;

	UPROPERTY()
	uint8 CurrentFireModeIdx;

	UPROPERTY(VisibleInstanceOnly, Replicated)
	uint8 CurrentAmmo;
	
	UPROPERTY(VisibleInstanceOnly, Replicated)
	uint8 OwnAmmo;
	
	UFUNCTION(Server, Reliable)
	void ServerRPCConsumingAmmo();

	UFUNCTION(Server, Reliable)
	void ServerRPCReloadingAmmo();
	
public:
	UFUNCTION(BlueprintCallable, Category = Weapon)
	void LoadWeaponStatData(const FName InName);

	UFUNCTION()
	void OnRep_SetWeaponStatData();

	UFUNCTION()
	void ConsumingAmmo();

	UFUNCTION()
	void ReloadingAmmo();

	FName WeaponName;
	
	UFUNCTION(BlueprintPure)
	FORCEINLINE USkeletalMeshComponent* GetMesh() const { return Mesh; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE UStaticMeshComponent* GetSightMesh() const { return Sight; }
	
};