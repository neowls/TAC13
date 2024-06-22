// Fill out your copyright notice in the Description page of Project Settings.


#include "TACCharacterBase.h"
#include "TAC13.h"
#include "TACCharacterMovementComponent.h"
#include "TACCharacterStatComponent.h"
#include "Animation/TACArmAnimInstance.h"
#include "Animation/TACBodyAnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"
#include "Game/Play/TACPlayGameMode.h"
#include "Game/Play/TACPlayGameState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerState.h"
#include "Input/TACControlData.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/TACWeapon.h"


// Sets default values
ATACCharacterBase::ATACCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UTACCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	InitializeObjectFinder(PlayControlData, TEXT("/Game/_TAC/Input/DA_Character.DA_Character"));
	InitializeObjectFinder(SpectateControlData, TEXT("/Game/_TAC/Input/DA_Spectate.DA_Spectate"));
	InitializeObjectFinder(FireArmMontage, TEXT("/Game/_Asset/Characters/Mannequin/Animations/Actions/AM_MM_Rifle_Fire.AM_MM_Rifle_Fire"));
	InitializeObjectFinder(FireCosmeticMontage, TEXT("/Game/_Asset/Characters/Mannequin/Animations/Actions/AM_MM_Rifle_Fire_Cosmetic.AM_MM_Rifle_Fire_Cosmetic"));
	InitializeObjectFinder(ChangeWeaponArmMontage, TEXT("/Game/_Asset/Characters/Mannequin/Animations/Actions/AM_Rifle_EquipOnly.AM_Rifle_EquipOnly"));
	InitializeObjectFinder(ChangeWeaponCosmeticMontage, TEXT("/Game/_Asset/Characters/Mannequin/Animations/Actions/AM_Rifle_EquipOnly_Cosmetic.AM_Rifle_EquipOnly_Cosmetic"));
	
	
	//	Movement Section
	TACCharacterMovement = Cast<UTACCharacterMovementComponent>(GetCharacterMovement());
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 45.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 600.0f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.0f;

	//	Camera Section
	FirstCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	FirstCamera->SetupAttachment(GetCapsuleComponent());
	CameraStandHeight = 80.f;
	CameraCrouchedHeight = 54.f;
	CameraProneHeight = 34.f;
	FirstCamera->SetRelativeLocation(FVector(0.f,0.f,CameraStandHeight));
	FirstCamera->bUsePawnControlRotation = true;

	//	Stat Component
	Stat = CreateDefaultSubobject<UTACCharacterStatComponent>(TEXT("Stat"));

	//	Montage
	FireMontage = FireCosmeticMontage;
	ChangeWeaponMontage = ChangeWeaponCosmeticMontage;

	bCanFire = true;
} 

void ATACCharacterBase::PostInitializeComponents()
{
	TAC_LOG(LogTACNetwork, Warning, TEXT("Start"));
	Super::PostInitializeComponents();
	Stat->OnHPZero.AddUObject(this, &ATACCharacterBase::SetDead);
	BodyAnimInstance = Cast<UTACBodyAnimInstance>(GetMesh()->GetAnimInstance());
	ArmAnimInstance = Cast<UTACArmAnimInstance>(ArmMesh->GetAnimInstance());
	TAC_LOG(LogTACNetwork, Warning, TEXT("Mesh Valid : %s"), GetMesh() ? TEXT("True") : TEXT("False"));
	if(HasAuthority())
	{
		SpawnWeapon("VAL");
		SpawnWeapon("KA47");
		SpawnWeapon("AR4");
	}
	TAC_LOG(LogTACNetwork, Warning, TEXT("End"));
}

void ATACCharacterBase::PostNetInit()
{
	TAC_LOG(LogTACNetwork, Warning, TEXT("Start"));
	Super::PostNetInit();
	TAC_LOG(LogTACNetwork, Warning, TEXT("End"));
}

void ATACCharacterBase::PossessedBy(AController* NewController)
{
	TAC_LOG(LogTACNetwork, Warning, TEXT("Start"));
	Super::PossessedBy(NewController);
	InitAttachWeapons();
	if(IsLocallyControlled()) ArmAnimInstance->SetNewSight();
	TAC_LOG(LogTACNetwork, Warning, TEXT("End"));
}

void ATACCharacterBase::OnRep_PlayerState()
{
	TAC_LOG(LogTACNetwork, Warning, TEXT("Start : %s") , *GetName());
	Super::OnRep_PlayerState();
	TAC_LOG(LogTACNetwork, Warning, TEXT("End : %s"), *GetName());
}

void ATACCharacterBase::BeginPlay()
{
	TAC_LOG(LogTACNetwork, Warning, TEXT("Start"));
	Super::BeginPlay();
	if(IsLocallyControlled()) ArmAnimInstance->SetNewSight();
	TAC_LOG(LogTACNetwork, Warning, TEXT("End"));
}

void ATACCharacterBase::Destroyed()
{
	Super::Destroyed();
	if(Stat)
	{
		Stat->OnHPChanged.Clear();		
	}
	

	if(!OwnWeapons.IsEmpty())
	{
		for(const auto iter : OwnWeapons)
		{
			iter->Destroy();
		}		
	}
}

void ATACCharacterBase::PlayMontageAnimation(const TObjectPtr<UAnimMontage>& MontageToPlay)
{
	if(IsLocallyControlled())
	{
		ArmAnimInstance->StopAllMontages(0.0f);
		ArmAnimInstance->Montage_Play(MontageToPlay);
	}
	else
	{
		BodyAnimInstance->StopAllMontages(0.0f);
		BodyAnimInstance->Montage_Play(MontageToPlay);
	}
	GetCurrentWeapon()->PlayFireAudio();
}

void ATACCharacterBase::SetCharacterControlData(const UTACControlData* CharacterControlData)
{
	bUseControllerRotationYaw = CharacterControlData->bUseControllerRotationYaw;

	GetCharacterMovement()->bOrientRotationToMovement = CharacterControlData->bOrientRotationToMovement;
	GetCharacterMovement()->bUseControllerDesiredRotation = CharacterControlData->bUseControllerDesiredRotation;
	GetCharacterMovement()->RotationRate = CharacterControlData->RotationRate;
}

void ATACCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ThisClass, bIsSprinting, COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(ThisClass, bIsADS, COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(ThisClass, CurrentWeaponIndex, COND_SkipOwner);
	DOREPLIFETIME(ThisClass, bCanFire);
	DOREPLIFETIME(ThisClass, OwnWeapons);
}

#pragma region RESPAWN

void ATACCharacterBase::SetDead()
{
	if(HasAuthority())
	{
		ATACPlayGameState* AGS = GetWorld()->GetGameState<ATACPlayGameState>();
		AGS->AddKillLogEntry(RecentAttacker->GetPlayerName(), RecentAttackedWeaponName, GetPlayerState()->GetPlayerName());
	}
	BodyAnimInstance->StopAllMontages(0.0f);
}

void ATACCharacterBase::RespawnCharacter()
{
	ATACPlayGameMode* TACGameMode = Cast<ATACPlayGameMode>(GetWorld()->GetAuthGameMode());
	if(TACGameMode)
	{
		TACGameMode->RespawnPlayer(GetController());
	}
	Destroy();
}

#pragma endregion

#pragma region ADS
void ATACCharacterBase::OnRep_IsADS()
{
	if(TACCharacterMovement)
	{
		if(bIsADS)
		{
			TACCharacterMovement->bWantsToADS = true;
			TACCharacterMovement->ADS(true);
		}
		else
		{
			TACCharacterMovement->bWantsToADS = false;
			TACCharacterMovement->UnADS(true);
		}
		
	}
}

void ATACCharacterBase::OnStartADS()
{
	
}

void ATACCharacterBase::OnEndADS()
{
	
}
#pragma endregion

#pragma region HIT

void ATACCharacterBase::FireHitCheck()
{
	const FVector Start = FirstCamera->GetComponentLocation();
	const FVector End = Start + FirstCamera->GetForwardVector() * 10000.0f;
	FHitResult HitResult;
	FWorldContext WorldContext;

	const TArray<AActor*> ActorsToIgnore;
	const bool bHit = UKismetSystemLibrary::LineTraceSingle(this, Start, End, UEngineTypes::ConvertToTraceType(ECC_Visibility),
		true, ActorsToIgnore, EDrawDebugTrace::ForDuration, HitResult, true, FLinearColor::Yellow, FLinearColor::Green, 2.0f);

	if(bHit)
	{
		GEngine->AddOnScreenDebugMessage(5, 2.0f, FColor::Yellow, FString::Printf(TEXT("Trace Hit : %s"), *HitResult.GetActor()->GetName()));
		GEngine->AddOnScreenDebugMessage(6, 2.0f, FColor::Red, FString::Printf(TEXT("Hit BoneName : %s"), *HitResult.BoneName.ToString()));
	}
}



void ATACCharacterBase::HitConfirm(const FHitResult& HitResult)
{
	if(HasAuthority())
	{
		float PartDamageMultiplier;
		if(HitResult.BoneName == TEXT("head")) PartDamageMultiplier = 2.5f;
		else PartDamageMultiplier = 1.0f;
		const float FireDamage = GetCurrentWeapon()->GetWeaponInfo().Damage * PartDamageMultiplier;
		FDamageEvent DamageEvent;
		HitResult.GetActor()->TakeDamage(FireDamage, DamageEvent, GetController(), GetCurrentWeapon().Get());
	}
}

float ATACCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	Stat->ApplyDamage(DamageAmount);
	RecentAttacker = Cast<ATACPlayPlayerState>(EventInstigator->PlayerState);
	RecentAttackedWeaponName = Cast<ATACWeapon>(DamageCauser)->GetWeaponName();
	return DamageAmount;
}

#pragma endregion

#pragma region WEAPON

void ATACCharacterBase::SpawnWeapon(const FName WeaponName)
{
	TAC_LOG(LogTACNetwork, Warning, TEXT("Start"));
	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.Instigator = GetInstigator();
	ATACWeapon* SpawnedWeapon = GetWorld()->SpawnActor<ATACWeapon>(WeaponToSpawn, Params);
	SpawnedWeapon->LoadWeaponInfoData(WeaponName);
	SpawnedWeapon->SetOwner(this);
	SpawnedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("Grip"));
	SpawnedWeapon->SetActorHiddenInGame(true);
	OwnWeapons.Add(SpawnedWeapon);
	TAC_LOG(LogTACNetwork, Warning, TEXT("End"));
}

void ATACCharacterBase::InitAttachWeapons()
{
	TAC_LOG(LogTACNetwork, Warning, TEXT("Start"));
	if(OwnWeapons.IsEmpty()) return;
	for(const auto iter : OwnWeapons)
	{
		TAC_LOG(LogTACNetwork, Warning, TEXT("Weapon Valid : %s"), iter ? TEXT("True") : TEXT("False"));
		if(IsValid(iter))
		{
			AttachWeapon(iter);
			//iter->SetOwner(this);
		}
	}
	TAC_LOG(LogTACNetwork, Warning, TEXT("End"));
}

void ATACCharacterBase::AttachWeapon(ATACWeapon* TargetWeapon) const
{
	if(IsLocallyControlled())
	{
		TAC_LOG(LogTACNetwork, Warning, TEXT("Attach to Arm"));
		TargetWeapon->AttachToComponent(ArmMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), FName("Grip"));		
	}
	else
	{
		TAC_LOG(LogTACNetwork, Warning, TEXT("Attach to Mesh"));
		TargetWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), FName("Grip"));
	}
}

void ATACCharacterBase::DropWeapon() // Need ServerSide Code
{
	GetCurrentWeapon()->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	GetCurrentWeapon()->GetMesh()->SetSimulatePhysics(true);
	GetCurrentWeapon()->GetMesh()->SetCollisionProfileName("Item");
	OwnWeapons.Remove(GetCurrentWeapon().Get());
	EquipWeapon(0);
}

void ATACCharacterBase::ChangeWeapon(const int8& ChangeValue) // CHANGE
{
	TAC_LOG(LogTACNetwork, Warning, TEXT("Start"));
	const int8 ChangeIndex = OwnWeapons.IsValidIndex(CurrentWeaponIndex + ChangeValue) ? CurrentWeaponIndex + ChangeValue : (ChangeValue > 0 ? 0 : OwnWeapons.Num() - 1);
	EquipWeapon(ChangeIndex);
}

void ATACCharacterBase::EquipWeapon(const uint8 NewWeaponIndex) // EQUIP
{
	if(!OwnWeapons.IsValidIndex(NewWeaponIndex) || !OwnWeapons[CurrentWeaponIndex]) return;
	PreviousWeaponIndex = CurrentWeaponIndex;
	if(!HasAuthority())
	{
		CurrentWeaponIndex = NewWeaponIndex;
		CurrentWeaponCosmetic();
	}
	
	Server_SetWeaponIndex(NewWeaponIndex);
}

void ATACCharacterBase::Server_SetWeaponIndex_Implementation(const uint8 NewWeaponIndex)
{
	TAC_LOG(LogTACNetwork, Warning, TEXT("Start"));
	CurrentWeaponIndex = NewWeaponIndex;
	OnRep_CurrentWeaponIndex();
}

void ATACCharacterBase::CurrentWeaponCosmetic() // EQUIP COSMETIC
{
	TAC_LOG(LogTACNetwork, Warning, TEXT("Start"));
	if(!OwnWeapons[PreviousWeaponIndex]) return;
	OwnWeapons[PreviousWeaponIndex]->SetActorHiddenInGame(true);
	PlayMontageAnimation(ChangeWeaponMontage);
	GetCurrentWeapon()->SetActorHiddenInGame(false);
	if(IsLocallyControlled())
	{
		OnCurrentAmmoChanged.Broadcast(GetCurrentWeapon()->GetCurrentAmmo());
		OnOwnAmmoChanged.Broadcast(GetCurrentWeapon()->GetOwnAmmo());
		OnWeaponNameChanged.Broadcast(GetCurrentWeapon()->GetWeaponName());
		ArmAnimInstance->SetNewSight();
	}
	TAC_LOG(LogTACNetwork, Warning, TEXT("End"));
}

void ATACCharacterBase::OnRep_CurrentWeaponIndex()
{
	CurrentWeaponCosmetic();
	PreviousWeaponIndex = CurrentWeaponIndex;
}

void ATACCharacterBase::OnRep_OwnWeapons()
{
	TAC_LOG(LogTACNetwork, Warning, TEXT("Start : %s"), *GetName());
	InitAttachWeapons();
	TAC_LOG(LogTACNetwork, Warning, TEXT("End : %s"), *GetName());
}

void ATACCharacterBase::Server_ReloadWeapon_Implementation()
{
	GetCurrentWeapon()->Reload();
}

void ATACCharacterBase::ReloadingWeapon()
{
	Server_ReloadWeapon();
}

void ATACCharacterBase::ChangeFireMode()
{
	GetCurrentWeapon()->ChangeFireMode();
}


#pragma endregion

#pragma region CROUCH
void ATACCharacterBase::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	TargetCameraHeight = CameraCrouchedHeight;
	GetWorldTimerManager().SetTimer(CrouchTimerHandle, this, &ATACCharacterBase::UpdateCameraHeight, 0.01f, true);
}

void ATACCharacterBase::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	TargetCameraHeight = CameraStandHeight;
	GetWorldTimerManager().SetTimer(CrouchTimerHandle, this, &ATACCharacterBase::UpdateCameraHeight, 0.01f, true);
}

void ATACCharacterBase::UpdateCameraHeight()
{
	float CurrentCameraHeight = FirstCamera->GetRelativeLocation().Z;
	CurrentCameraHeight = UKismetMathLibrary::FInterpTo(CurrentCameraHeight, TargetCameraHeight, GetWorld()->GetDeltaSeconds(), 10.0f);
	FirstCamera->SetRelativeLocation(FVector(10.f,0.f, CurrentCameraHeight));
	if(abs(TargetCameraHeight - CurrentCameraHeight) < 0.01f )
	{
		FirstCamera->SetRelativeLocation(FVector(10.f,0.f, TargetCameraHeight));
		GetWorldTimerManager().ClearTimer(CrouchTimerHandle);
	}
}
#pragma endregion

#pragma region SPRINT
void ATACCharacterBase::OnRep_IsSprinting()
{
	if (TACCharacterMovement)
	{
		if (bIsSprinting)
		{
			TACCharacterMovement->bWantsToCrouch = false;
			TACCharacterMovement->bWantsToSprint = true;
			TACCharacterMovement->bWantsToADS = false;
			TACCharacterMovement->Sprint(true);
		}
		else
		{
			TACCharacterMovement->bWantsToSprint = false;
			TACCharacterMovement->UnSprint(true);
		}
		TACCharacterMovement->bNetworkUpdateReceived = true;
	}
	
}

void ATACCharacterBase::Sprint()
{
	if (TACCharacterMovement)
	{
		TACCharacterMovement->bWantsToCrouch = false;
		TACCharacterMovement->bWantsToSprint = true;
	}
}

void ATACCharacterBase::UnSprint()
{
	if (TACCharacterMovement)
	{
		TACCharacterMovement->bWantsToSprint = false;
	}
}

void ATACCharacterBase::OnEndSprint()
{
	GetTACCharacterMovement()->MaxWalkSpeed = GetTACCharacterMovement()->MaxBaseSpeed;
}

void ATACCharacterBase::OnStartSprint()
{
	GetTACCharacterMovement()->MaxWalkSpeed = GetTACCharacterMovement()->MaxWalkSpeedSprinting;
}

#pragma endregion 
