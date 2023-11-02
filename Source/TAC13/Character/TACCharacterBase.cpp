// Fill out your copyright notice in the Description page of Project Settings.


#include "TACCharacterBase.h"

#include "TACCharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Input/TACControlData.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ATACCharacterBase::ATACCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UTACCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	static ConstructorHelpers::FObjectFinder<UTACControlData> ControlDataRef(TEXT("/Script/TAC13.TACControlData'/Game/_TAC/Input/DA_Character.DA_Character'"));
	if (nullptr != ControlDataRef.Object)
	{
		CurrentControlData = ControlDataRef.Object;
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

	
	//	Camera Section
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(GetCapsuleComponent());
	Camera->SetRelativeLocation(FVector(0.f,0.f,80.f));
	Camera->bUsePawnControlRotation = true;
	
	//	Movement Section
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 45.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 700.0f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;
	TACCharacterMovement = Cast<UTACCharacterMovementComponent>(GetCharacterMovement());
	PronedEyeHeight = 30.f;
}

void ATACCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
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
	
	DOREPLIFETIME_CONDITION(ThisClass, bIsProned, COND_SimulatedOnly);
}

void ATACCharacterBase::FireHitCheck()
{
	const FVector Start = Camera->GetComponentLocation();
	const FVector End = Start + Camera->GetForwardVector() * 10000.0f;
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

void ATACCharacterBase::RecalculateBaseEyeHeight()
{
	if (bIsProned)
	{
		BaseEyeHeight = PronedEyeHeight;
	}
	else
	{
		Super::RecalculateBaseEyeHeight();
	}
}

void ATACCharacterBase::OnRep_IsProned()
{
	if (TACCharacterMovement)
	{
		if (bIsProned)
		{
			TACCharacterMovement->bWantsToProne = true;
			TACCharacterMovement->Prone(true);
		}
		else
		{
			TACCharacterMovement->bWantsToProne = false;
			TACCharacterMovement->UnProne(true);
		}
		TACCharacterMovement->bNetworkUpdateReceived = true;
	}
}

void ATACCharacterBase::Prone(bool bClientSimulation)
{
	if (TACCharacterMovement)
	{
		if (CanProne())
		{
			TACCharacterMovement->bWantsToProne = true;
		}
	}
}

void ATACCharacterBase::UnProne(bool bClientSimulation)
{
	if (TACCharacterMovement)
	{
		TACCharacterMovement->bWantsToProne = false;
	}
}

bool ATACCharacterBase::CanProne() const
{
	return !bIsProned && GetRootComponent() && !GetRootComponent()->IsSimulatingPhysics();
}

void ATACCharacterBase::OnEndProne(float HeightAdjust, float ScaledHeightAdjust)
{
	RecalculateBaseEyeHeight();

	if (!bIsCrouched)
	{
		const ACharacter* DefaultChar = GetDefault<ACharacter>(GetClass());
		if (GetMesh() && DefaultChar->GetMesh())
		{
			FVector& MeshRelativeLocation = GetMesh()->GetRelativeLocation_DirectMutable();
			MeshRelativeLocation.Z = DefaultChar->GetMesh()->GetRelativeLocation().Z;
			BaseTranslationOffset.Z = MeshRelativeLocation.Z;
		}
		else
		{
			BaseTranslationOffset.Z = DefaultChar->GetBaseTranslationOffset().Z;
		}
	}
	K2_OnEndProne(HeightAdjust, ScaledHeightAdjust);
}

void ATACCharacterBase::OnStartProne(float HeightAdjust, float ScaledHeightAdjust)
{
	RecalculateBaseEyeHeight();

	const ACharacter* DefaultChar = GetDefault<ACharacter>(GetClass());
	if (GetMesh() && DefaultChar->GetMesh())
	{
		FVector& MeshRelativeLocation = GetMesh()->GetRelativeLocation_DirectMutable();
		MeshRelativeLocation.Z = DefaultChar->GetMesh()->GetRelativeLocation().Z + HeightAdjust;
		BaseTranslationOffset.Z = MeshRelativeLocation.Z;
	}
	else
	{
		BaseTranslationOffset.Z = DefaultChar->GetBaseTranslationOffset().Z + HeightAdjust;
	}

	K2_OnStartProne(HeightAdjust, ScaledHeightAdjust);
}