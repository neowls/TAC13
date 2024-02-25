// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TACWeaponComponent.generated.h"


UCLASS()
class TAC13_API UTACWeaponComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:	
	UTACWeaponComponent();

protected:
	virtual void BeginPlay() override;

public:
	
};
