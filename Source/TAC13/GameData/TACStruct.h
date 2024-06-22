#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
//#include "Weapon/TACWeapon.h"
#include "TACStruct.generated.h"


UENUM(BlueprintType)
enum class EFireMode:uint8		//	조정간 모드
{
	SINGLE,
	BURST,
	FULL
};

USTRUCT(BlueprintType)
struct FPlayerScore				//	플레이어 점수
{
	GENERATED_BODY()

	FPlayerScore() : PlayerName("Player"), KillScore(0), DeathScore(0) {};
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString PlayerName;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 KillScore;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 DeathScore;
};

USTRUCT(BlueprintType)
struct FKillLogEntry			//	킬로그 정보
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString KillerName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString VictimName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString WeaponName;
};


USTRUCT(BlueprintType)
struct FTACAttachment : public FTableRowBase	//	부착물 정보
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
	TObjectPtr<class UStaticMesh> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Info)
	FString Name;
};

USTRUCT(BlueprintType)
struct FTACWeaponStat	//	총기 상태
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
struct FTACWeaponInfo : public FTableRowBase	//	총기 정보
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


USTRUCT()
struct FLoadOut
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TObjectPtr<class ATACWeapon> PrimaryWeapon;

	UPROPERTY()
	TObjectPtr<class ATACWeapon> SecondaryWeapon;

	UPROPERTY()
	FString ArmorType;
	
};
