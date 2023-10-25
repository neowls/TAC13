// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/TACAnimationFireInterface.h"
#include "TACCharacterBase.generated.h"

UCLASS()
class TAC13_API ATACCharacterBase : public ACharacter, public ITACAnimationFireInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATACCharacterBase(const FObjectInitializer& ObjectInitializer);

	/** Returns CameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetCameraComponent() const { return Camera; }

	virtual void PostInitializeComponents() override;
	
protected:
	virtual void SetCharacterControlData(const class UTACControlData* CharacterControlData);

	UPROPERTY(EditAnywhere, Category = CharacterControl, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTACControlData> CurrentControlData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	TObjectPtr<class UCameraComponent> Camera;

// Fire Montage
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> FireArmMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> FireCosmeticMontage;

// Hit Section
protected:
	virtual void FireHitCheck() override;
};
