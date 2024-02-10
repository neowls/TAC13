// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TACCharacterStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHPZeroDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHPChangedDelegate, uint8 /*CurrentHP*/);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TAC13_API UTACCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UTACCharacterStatComponent();

protected:

	virtual void InitializeComponent() override;

public:
	FOnHPZeroDelegate OnHPZero;
	FOnHPChangedDelegate OnHPChanged;
	
	FORCEINLINE uint8 GetMaxHP() { return MaxHP; }
	FORCEINLINE uint8 GetCurrentHP() { return CurrentHP; }
	
	float ApplyDamage(uint8 InDamage);
	
	void SetHP(int8 NewHP);
protected:
	virtual void ReadyForReplication() override;
	

	
	UPROPERTY(ReplicatedUsing = OnRep_CurrentHP, Transient, VisibleInstanceOnly, Category = Stat)
	uint8 CurrentHP;
	
	UPROPERTY(VisibleInstanceOnly, Category = Stat)
	uint8 MaxHP;


protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_CurrentHP();
		
};
