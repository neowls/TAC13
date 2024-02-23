// Fill out your copyright notice in the Description page of Project Settings.


#include "TACCharacterBase.h"
#include "TAC13.h"
#include "TACCharacterMovementComponent.h"
#include "TACCharacterStatComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Game/TACGameState.h"
#include "Game/TACPlayerState.h"
#include "GameData/TACGameSingleton.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerState.h"
#include "Input/TACControlData.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/TACWeapon.h"


// Sets default values
ATACCharacterBase::ATACCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UTACCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	static ConstructorHelpers::FObjectFinder<UTACControlData> PlayControlDataRef(TEXT("/Script/TAC13.TACControlData'/Game/_TAC/Input/DA_Character.DA_Character'"));
	if (nullptr != PlayControlDataRef.Object)
	{
		PlayControlData = PlayControlDataRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UTACControlData> SpectateControlDataRef(TEXT("/Script/TAC13.TACControlData'/Game/_TAC/Input/DA_Spectate.DA_Spectate'"));
	if (nullptr != SpectateControlDataRef.Object)
	{
		SpectateControlData = SpectateControlDataRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> FireMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/_Asset/Characters/Mannequin/Animations/Actions/AM_MM_Rifle_Fire.AM_MM_Rifle_Fire'"));
	if (FireMontageRef.Object)
	{
		FireArmMontage = FireMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> FireCosmeticMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/_Asset/Characters/Mannequin/Animations/Actions/AM_MM_Rifle_Fire_Cosmetic.AM_MM_Rifle_Fire_Cosmetic'"));
	if (FireCosmeticMontageRef.Object)
	{
		FireCosmeticMontage = FireCosmeticMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> ChangeWeaponMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/_Asset/Characters/Mannequin/Animations/Actions/AM_Rifle_Equip_Montage.AM_Rifle_Equip_Montage'"));
	if (ChangeWeaponMontageRef.Object)
	{
		ChangeWeaponMontage = ChangeWeaponMontageRef.Object;
	}
	
	//	Movement Section
	TACCharacterMovement = Cast<UTACCharacterMovementComponent>(GetCharacterMovement());
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 45.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 700.0f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;

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
} 

void ATACCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	Stat->OnHPZero.AddUObject(this, &ATACCharacterBase::SetDead);
	InitialMeshTransform = GetMesh()->GetRelativeTransform();
}

float ATACCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	Stat->ApplyDamage(DamageAmount);
	RecentAttacker = Cast<ATACPlayerState>(EventInstigator->PlayerState);
	RecentAttackedWeaponName = Cast<ATACWeapon>(DamageCauser)->GetWeaponName();
	return DamageAmount;
}

void ATACCharacterBase::SetDead()
{
	TAC_LOG(LogTACNetwork, Log, TEXT("Character Dead"));
	PlayDeadAnimation();
	ATACGameState* AGS = GetWorld()->GetGameState<ATACGameState>();
	if(HasAuthority())
	{
		AGS->AddKillLogEntry(RecentAttacker->GetPlayerName(), RecentAttackedWeaponName, GetPlayerState()->GetPlayerName());
	}
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
	
}

void ATACCharacterBase::OnEndADS()
{
	
}

void ATACCharacterBase::ChangeWeaponCheck()
{
	
}

void ATACCharacterBase::PlayDeadAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->StopAllMontages(0.0f);
	//AnimInstance->Montage_Play(DeadMontage, 1.0f);
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


#pragma region 웅크리기
void ATACCharacterBase::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	FirstCamera->SetRelativeLocation(FVector(10.f,0.f, CameraCrouchedHeight));
}

void ATACCharacterBase::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	FirstCamera->SetRelativeLocation(FVector(10.f,0.f, CameraStandHeight));
	TAC_LOG(LogTACNetwork, Log, TEXT("%s"), TEXT("Begin"));
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

void ATACCharacterBase::Sprint(bool bClientSimulation)
{
	TAC_LOG(LogTACNetwork, Log, TEXT("%s"), TEXT("Begin"));
	if (TACCharacterMovement)
	{
		TACCharacterMovement->bWantsToCrouch = false;
		TACCharacterMovement->bWantsToSprint = true;
	}
	TAC_LOG(LogTACNetwork, Log, TEXT("%s"), TEXT("End"));
}

void ATACCharacterBase::UnSprint(bool bClientSimulation)
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
	
	K2_OnEndSprint();
}

void ATACCharacterBase::OnStartSprint()
{
	GetTACCharacterMovement()->MaxWalkSpeed = GetTACCharacterMovement()->MaxWalkSpeedSprinting;
	K2_OnStartSprint();
	
}

#pragma endregion 

void ATACCharacterBase::SetupCharacterWidget(UTACUserWidget* InUserWidget)
{

}