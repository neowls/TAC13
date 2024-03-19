#include "Weapon/TACWeapon.h"
#include "TAC13.h"
#include "Character/TACCharacterBase.h"
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

	bReplicates = true;
}

void ATACWeapon::SetOwner(AActor* NewOwner)
{
	Super::SetOwner(NewOwner);
	CurrentOwner = Cast<ATACCharacterBase>(NewOwner);
}

void ATACWeapon::ChangeFireMode()
{
	if(WeaponInfo.OwnFireMode.Num() > 1)
	{
		CurrentFireModeIdx == WeaponInfo.OwnFireMode.Num()-1 ? CurrentFireModeIdx = 0 : CurrentFireModeIdx++;
		WeaponStat.CurrentFireMode = WeaponInfo.OwnFireMode[CurrentFireModeIdx];
	}
}


void ATACWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATACWeapon, WeaponInfo);
	DOREPLIFETIME(ATACWeapon, WeaponStat);
}

void ATACWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void ATACWeapon::InitWeaponStat()
{
	WeaponStat.CurrentFireMode = WeaponInfo.OwnFireMode[0];
	WeaponStat.OwnAmmo = WeaponInfo.MaxAmmo * 6;
	WeaponStat.CurrentAmmo = WeaponInfo.MaxAmmo;
}

void ATACWeapon::UseAmmo()
{
	if(WeaponStat.CurrentAmmo <= 0) return;
	WeaponStat.CurrentAmmo--;
	TAC_LOG(LogTACNetwork, Log, TEXT("Current Ammo : %d"), WeaponStat.CurrentAmmo);
	CurrentOwner->OnCurrentAmmoChanged.Broadcast(WeaponStat.CurrentAmmo);
}

void ATACWeapon::Reload()
{
	if(WeaponStat.OwnAmmo <= 0 || WeaponInfo.MaxAmmo == WeaponStat.CurrentAmmo)
	{
		TAC_LOG(LogTACNetwork, Log, TEXT("Can't Reload"));
		return;
	}
	const uint8 WantAmmo = WeaponInfo.MaxAmmo - WeaponStat.CurrentAmmo;
	const uint8 RealLoadAmmo = WantAmmo > WeaponStat.OwnAmmo ? WeaponStat.OwnAmmo : WantAmmo;
	WeaponStat.OwnAmmo -= RealLoadAmmo;
	WeaponStat.CurrentAmmo += RealLoadAmmo;

	TAC_LOG(LogTACNetwork, Log, TEXT("Current Ammo : %d"), WeaponStat.CurrentAmmo);

	CurrentOwner->OnCurrentAmmoChanged.Broadcast(WeaponStat.CurrentAmmo);
	CurrentOwner->OnOwnAmmoChanged.Broadcast(WeaponStat.OwnAmmo);
}

void ATACWeapon::LoadWeaponInfoData(const FName InName)
{
	TAC_LOG(LogTACNetwork, Log, TEXT("Begin"));
	WeaponInfo = UTACGameSingleton::Get().GetWeaponInfoData(InName);
	WeaponInfo.Name = InName.ToString();
	OnRep_WeaponInfo();
	InitWeaponStat();
}

void ATACWeapon::OnRep_WeaponInfo()
{
	Mesh->SetSkeletalMesh(WeaponInfo.BodyMesh);
}

void ATACWeapon::OnRep_WeaponStat()
{
	CurrentOwner->OnCurrentAmmoChanged.Broadcast(WeaponStat.CurrentAmmo);
	CurrentOwner->OnOwnAmmoChanged.Broadcast(WeaponStat.OwnAmmo);
}
