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
#include "Game/TACGameState.h"
#include "Game/TACPlayerState.h"
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
	Super::PostInitializeComponents();
	Stat->OnHPZero.AddUObject(this, &ATACCharacterBase::SetDead);
	BodyAnimInstance = Cast<UTACBodyAnimInstance>(GetMesh()->GetAnimInstance());
}


void ATACCharacterBase::HitConfirm(const FHitResult& HitResult)
{
	if(HasAuthority())
	{
		float PartDamageMultiplier;
		if(HitResult.BoneName == TEXT("head")) PartDamageMultiplier = 2.5f;
		else PartDamageMultiplier = 1.0f;
		const float FireDamage = CurrentWeapon->GetWeaponInfo().Damage * PartDamageMultiplier;
		FDamageEvent DamageEvent;
		HitResult.GetActor()->TakeDamage(FireDamage, DamageEvent, GetController(), CurrentWeapon.Get());
	}
}



float ATACCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	Stat->ApplyDamage(DamageAmount);
	RecentAttacker = Cast<ATACPlayerState>(EventInstigator->PlayerState);
	RecentAttackedWeaponName = Cast<ATACWeapon>(DamageCauser)->GetWeaponName();
	return DamageAmount;
}

void ATACCharacterBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	LOG_SCREEN(0, TEXT("Camera Height : %f"), FirstCamera->GetRelativeLocation().Z);
}

void ATACCharacterBase::SetDead()
{
	TAC_LOG(LogTACNetwork, Log, TEXT("Character Dead"));
	ATACGameState* AGS = GetWorld()->GetGameState<ATACGameState>();
	if(HasAuthority())
	{
		AGS->AddKillLogEntry(RecentAttacker->GetPlayerName(), RecentAttackedWeaponName, GetPlayerState()->GetPlayerName());
	}
	BodyAnimInstance->StopAllMontages(0.0f);
}

void ATACCharacterBase::RespawnCharacter()
{
	
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
	DOREPLIFETIME(ThisClass, bCanFire);
	DOREPLIFETIME(ThisClass, CurrentWeaponIndex);
	DOREPLIFETIME(ThisClass, OwnWeapons);
}

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
	TAC_LOG(LogTACNetwork, Log, TEXT("begin"));
}

void ATACCharacterBase::OnEndADS()
{
	TAC_LOG(LogTACNetwork, Log, TEXT("begin"));
}

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

#pragma region WEAPON

void ATACCharacterBase::SpawnWeapon(const FName WeaponName)
{
	FActorSpawnParameters Params;
	Params.Owner = this;
	ATACWeapon* SpawnedWeapon = GetWorld()->SpawnActor<ATACWeapon>(WeaponToSpawn, Params);
	SpawnedWeapon->LoadWeaponInfoData(WeaponName);
	SpawnedWeapon->SetOwner(this);
	SpawnedWeapon->SetActorHiddenInGame(true);
	AttachWeapon(SpawnedWeapon);
	OwnWeapons.Add(SpawnedWeapon);
}

void ATACCharacterBase::AttachWeapon(ATACWeapon* TargetWeapon)
{
	if(IsLocallyControlled())
	TargetWeapon->AttachToComponent(ArmMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), FName("Grip"));
	else
	{
		TargetWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), FName("Grip"));
	}
}

void ATACCharacterBase::DropWeapon()
{
	OwnWeapons.Remove(CurrentWeapon.Get());
	CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	CurrentWeapon->GetMesh()->SetSimulatePhysics(true);
	CurrentWeapon->GetMesh()->SetCollisionProfileName("Item");
	ApplyWeaponChange(0);
}

void ATACCharacterBase::ReloadingWeapon()
{
	ServerReloadWeapon();
}


void ATACCharacterBase::CurrentWeaponCosmetic()
{
	PlayMontageAnimation(ChangeWeaponMontage);
	CurrentWeapon->SetActorHiddenInGame(false);
	OnCurrentAmmoChanged.Broadcast(CurrentWeapon->GetCurrentAmmo());
	OnOwnAmmoChanged.Broadcast(CurrentWeapon->GetOwnAmmo());
	OnWeaponNameChanged.Broadcast(CurrentWeapon->GetWeaponName());		
}

void ATACCharacterBase::OnRep_OwnWeapons()
{
	if(OwnWeapons.IsEmpty()) return;
	for(const auto iter : OwnWeapons)
	{
		if(IsValid(iter))
			AttachWeapon(iter);
	}
}

void ATACCharacterBase::ServerReloadWeapon_Implementation()
{
	CurrentWeapon->Reload();
}

void ATACCharacterBase::ChangeWeapon(const int8& ChangeValue)
{
	const int8 ChangeIndex = OwnWeapons.IsValidIndex(CurrentWeaponIndex + ChangeValue) ? CurrentWeaponIndex + ChangeValue : (ChangeValue > 0 ? 0 : OwnWeapons.Num() - 1);
	ApplyWeaponChange(ChangeIndex);
	ServerChangeWeapon(ChangeIndex);
}

void ATACCharacterBase::ApplyWeaponChange(const uint8 NewWeaponIndex)
{
	if(!OwnWeapons.IsValidIndex(NewWeaponIndex) || CurrentWeapon == OwnWeapons[NewWeaponIndex]) return;
	if(CurrentWeapon) CurrentWeapon->SetActorHiddenInGame(true);
	CurrentWeaponIndex = NewWeaponIndex;
	CurrentWeapon = OwnWeapons[NewWeaponIndex];
	FireTime = 60 / CurrentWeapon->GetWeaponInfo().FireRate;
	CurrentWeaponCosmetic();
	ArmAnimInstance->SetNewSight();
}


void ATACCharacterBase::ChangeWeaponCheck()
{
	
}

void ATACCharacterBase::OnRep_CurrentWeaponIndex()
{
	ApplyWeaponChange(CurrentWeaponIndex);
}

void ATACCharacterBase::OnRep_CanFire()
{
	
}

void ATACCharacterBase::ServerChangeWeapon_Implementation(const uint8 Index)
{
	CurrentWeaponIndex = Index;
	ApplyWeaponChange(Index);
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
}

void ATACCharacterBase::ChangeFireMode()
{
	CurrentWeapon->ChangeFireMode();
}

#pragma endregion

#pragma region 웅크리기
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

#pragma region 달리기

void ATACCharacterBase::OnRep_IsSprinting()
{
	TAC_LOG(LogTACNetwork, Log, TEXT("%s"), TEXT("begin"));
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
	TAC_LOG(LogTACNetwork, Log, TEXT("%s"), TEXT("end"));
}

void ATACCharacterBase::Sprint()
{
	TAC_LOG(LogTACNetwork, Log, TEXT("%s"), TEXT("Begin"));
	if (TACCharacterMovement)
	{
		TACCharacterMovement->bWantsToCrouch = false;
		TACCharacterMovement->bWantsToSprint = true;
	}
	TAC_LOG(LogTACNetwork, Log, TEXT("%s"), TEXT("End"));
}

void ATACCharacterBase::UnSprint()
{
	TAC_LOG(LogTACNetwork, Log, TEXT("%s"), TEXT("Begin"));
	if (TACCharacterMovement)
	{
		TACCharacterMovement->bWantsToSprint = false;
	}
	TAC_LOG(LogTACNetwork, Log, TEXT("%s"), TEXT("End"));
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

void ATACCharacterBase::SetupCharacterWidget(UTACUserWidget* InUserWidget)
{

}