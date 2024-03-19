// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "TACWeapon.generated.h"

class ATACCharacterBase;

UENUM(BlueprintType)
enum class EFireMode:uint8
{
	SINGLE,
	BURST,
	FULL
};

USTRUCT(BlueprintType)
struct FTACAttachment : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
	TObjectPtr<class UStaticMesh> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Info)
	FString Name;
};

USTRUCT(BlueprintType)
struct FTACWeaponStat
{
	GENERATED_BODY()

	UPROPERTY(VisibleInstanceOnly)
	uint8 OwnAmmo = 0;
	
	UPROPERTY(VisibleInstanceOnly)
	uint8 CurrentAmmo = 0;
	
	UPROPERTY(VisibleInstanceOnly)
	EFireMode CurrentFireMode = EFireMode::SINGLE;

};


USTRUCT(BlueprintType)
struct FTACWeaponInfo : public FTableRowBase
{
	GENERATED_BODY()

public:
	FTACWeaponInfo() : Damage(0.0f), FireRate(0.0f), Recoil(0.0f), Ergonomics(0.0f), MaxAmmo(0) {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Info)
	TObjectPtr<class USkeletalMesh> BodyMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Info)
	TArray<EFireMode> OwnFireMode;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Info)
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Info)
	float FireRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Info)
	float Recoil;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Info)
	float Ergonomics;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Info)
	uint8 MaxAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Info)
	FString Name;
};




UCLASS()
class TAC13_API ATACWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	ATACWeapon();


	FORCEINLINE FTACWeaponInfo GetWeaponInfo() const { return WeaponInfo; }
	FORCEINLINE FTACWeaponStat GetWeaponStat() const { return WeaponStat; }
	void SetWeaponStat(const FTACWeaponInfo& InWeaponInfo) { WeaponInfo = InWeaponInfo; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE uint8 GetCurrentAmmo() const { return WeaponStat.CurrentAmmo; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE uint8 GetOwnAmmo() const { return WeaponStat.OwnAmmo; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE FString GetWeaponName() const { return WeaponInfo.Name; }

	virtual void SetOwner(AActor* NewOwner) override;

	void ChangeFireMode();
	

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	TObjectPtr<USceneComponent> Root;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Weapon)
	TObjectPtr<USkeletalMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	TObjectPtr<UStaticMeshComponent> Sight;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = Weapon)
	TWeakObjectPtr<ATACCharacterBase> CurrentOwner;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_WeaponInfo , Category = Info)
	FTACWeaponInfo WeaponInfo;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, ReplicatedUsing = OnRep_WeaponStat)
	FTACWeaponStat WeaponStat;
	
	UPROPERTY()
	uint8 CurrentFireModeIdx = 0;
	
public:
	void InitWeaponStat();

	void UseAmmo();

	void Reload();
	
	UFUNCTION(Category = Weapon)
	void LoadWeaponInfoData(const FName InName);

	UFUNCTION()
	void OnRep_WeaponInfo();

	UFUNCTION()
	void OnRep_WeaponStat();
	
	UFUNCTION(BlueprintPure)
	FORCEINLINE USkeletalMeshComponent* GetMesh() const { return Mesh; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE UStaticMeshComponent* GetSightMesh() const { return Sight; }
	
};