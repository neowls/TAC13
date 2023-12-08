#include "Weapon/TACWeapon.h"

#include "TAC13.h"
#include "Character/TACCharacterPlayer.h"
#include "GameData/TACGameSingleton.h"
#include "Net/UnrealNetwork.h"

ATACWeapon::ATACWeapon()
{
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
	
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);
	Mesh->CastShadow = false;
	
	Sight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Optic"));
	Sight->SetupAttachment(Mesh, "Sight");
	Sight->CastShadow = false;
	
}

void ATACWeapon::SetOwner(AActor* NewOwner)
{
	Super::SetOwner(NewOwner);
	CurrentOwner = Cast<ATACCharacterPlayer>(NewOwner);
}

void ATACWeapon::ChangeFireMode()
{
	if(WeaponStat.OwnFireMode.Num() > 1)
	{
		CurrentFireModeIdx == WeaponStat.OwnFireMode.Num()-1 ? CurrentFireModeIdx = 0 : CurrentFireModeIdx++;
		CurrentFireMode = WeaponStat.OwnFireMode[CurrentFireModeIdx];
	}
	TAC_LOG(LogTACNetwork, Log, TEXT("Current Fire Mode : %hhd"), CurrentFireMode);
}

void ATACWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATACWeapon, WeaponStat);
	DOREPLIFETIME(ATACWeapon, OwnAmmo);
	DOREPLIFETIME(ATACWeapon, CurrentAmmo);
}


void ATACWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void ATACWeapon::ConsumingAmmo()
{
	if(CurrentOwner->IsLocallyControlled())
	ServerRPCConsumingAmmo();
}

void ATACWeapon::ReloadingAmmo()
{
	if(CurrentOwner->IsLocallyControlled())
	ServerRPCReloadingAmmo();
}

void ATACWeapon::ServerRPCReloadingAmmo_Implementation()
{
	if(OwnAmmo <= 0 || WeaponStat.MaxAmmo == CurrentAmmo)
	{
		TAC_LOG(LogTACNetwork, Log, TEXT("No Ammo"));
		return;
	}
	const uint8 WantAmmo = WeaponStat.MaxAmmo - CurrentAmmo;
	const uint8 RealLoadAmmo = WantAmmo > OwnAmmo ? OwnAmmo : WantAmmo;
	OwnAmmo -= RealLoadAmmo;
	CurrentAmmo += RealLoadAmmo;
}

void ATACWeapon::ServerRPCConsumingAmmo_Implementation()
{
	if(CurrentAmmo <= 0)
	{
		TAC_LOG(LogTACNetwork, Log, TEXT("Magazine is Empty. Please Reload"));
		return;
	}
	CurrentAmmo--;
}

void ATACWeapon::LoadWeaponStatData(const FName InName)
{
	TAC_LOG(LogTACNetwork, Log, TEXT("Begin"));
	WeaponStat = UTACGameSingleton::Get().GetWeaponStatData(InName);
	WeaponName = InName;
	OwnAmmo = WeaponStat.MaxAmmo * 6;
	CurrentAmmo = WeaponStat.MaxAmmo;
	OnRep_SetWeaponStatData();
}

void ATACWeapon::OnRep_SetWeaponStatData()
{
	Mesh->SetSkeletalMesh(WeaponStat.OwnMesh);
	CurrentFireMode = WeaponStat.OwnFireMode[0];
	CurrentFireModeIdx = 0;
}