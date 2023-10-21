// Fill out your copyright notice in the Description page of Project Settings.


#include "TACCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Input/TACControlData.h"

// Sets default values
ATACCharacterBase::ATACCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UTACControlData> ControlDataRef(TEXT("/Script/TAC13.TACControlData'/Game/_TAC/Input/DA_Character.DA_Character'"));
	if (nullptr != ControlDataRef.Object)
	{
		CurrentControlData = ControlDataRef.Object;
	}
	
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
