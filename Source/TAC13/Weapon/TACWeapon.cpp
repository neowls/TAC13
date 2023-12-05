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
}


void ATACWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void ATACWeapon::LoadWeaponStatData(FName InName)
{
	WeaponStat = UTACGameSingleton::Get().GetWeaponStatData(InName);
	OnRep_SetWeaponStatData();
}

void ATACWeapon::OnRep_SetWeaponStatData()
{
	TAC_LOG(LogTACNetwork, Log, TEXT("%s"), TEXT("Begin"));
	Mesh->SetSkeletalMesh(WeaponStat.OwnMesh);
	CurrentFireMode = WeaponStat.OwnFireMode[0];
	CurrentFireModeIdx = 0;
	CurrentAmmo = WeaponStat.MaxAmmo;
	OwnAmmo = WeaponStat.MaxAmmo * 5;
	TAC_LOG(LogTACNetwork, Log, TEXT("%s"), TEXT("End"));
}