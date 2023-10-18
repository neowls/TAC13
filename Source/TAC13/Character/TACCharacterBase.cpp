// Fill out your copyright notice in the Description page of Project Settings.


#include "TACCharacterBase.h"

// Sets default values
ATACCharacterBase::ATACCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATACCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATACCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATACCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

