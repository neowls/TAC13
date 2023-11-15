// Fill out your copyright notice in the Description page of Project Settings.


#include "TACCharacterBase.h"

#include "TAC13.h"
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
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(GetCapsuleComponent());
	CameraStandHeight = 80.f;
	CameraCrouchedHeight = 54.f;
	CameraProneHeight = 34.f;
	Camera->SetRelativeLocation(FVector(0.f,0.f,CameraStandHeight));
	Camera->bUsePawnControlRotation = true;
	
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
	DOREPLIFETIME_CONDITION(ThisClass, bIsSprinting, COND_SimulatedOnly);
}

#pragma region 웅크리기
void ATACCharacterBase::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	Camera->SetRelativeLocation(FVector(10.f,0.f, CameraCrouchedHeight));
}

void ATACCharacterBase::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	Camera->SetRelativeLocation(FVector(10.f,0.f, CameraStandHeight));
}
#pragma endregion

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
	return !bIsProned && !bIsSprinting && GetRootComponent() && !GetRootComponent()->IsSimulatingPhysics();
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
	Camera->SetRelativeLocation(FVector(10.f,0.f, CameraStandHeight));
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
	Camera->SetRelativeLocation(FVector(10.f,0.f, CameraProneHeight));
	K2_OnStartProne(HeightAdjust, ScaledHeightAdjust);
}

void ATACCharacterBase::OnRep_IsSprinting()
{
	if (TACCharacterMovement)
	{
		if (bIsSprinting)
		{
			TACCharacterMovement->bWantsToProne = false;
			TACCharacterMovement->bWantsToCrouch = false;
			TACCharacterMovement->bWantsToSprint = true;
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

void ATACCharacterBase::Sprint(bool bClientSimulation)
{
	TAC_LOG(LogTACNetwork, Log, TEXT("%s"), TEXT("Begin"));
	if (TACCharacterMovement)
	{
		TACCharacterMovement->bWantsToProne = false;
		TACCharacterMovement->bWantsToCrouch = false;
		TACCharacterMovement->bWantsToSprint = true;
	}
}

void ATACCharacterBase::UnSprint(bool bClientSimulation)
{
	TAC_LOG(LogTACNetwork, Log, TEXT("%s"), TEXT("Begin"));
	if (TACCharacterMovement)
	{
		TACCharacterMovement->bWantsToSprint = false;
	}
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