
#include "Character/TACCharacterStatComponent.h"
#include "TAC13.h"
#include "Net/UnrealNetwork.h"


UTACCharacterStatComponent::UTACCharacterStatComponent()
{
	MaxHP = 100;
	bWantsInitializeComponent = true;

	SetIsReplicatedByDefault(true);
}

void UTACCharacterStatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UTACCharacterStatComponent, CurrentHP);
}

void UTACCharacterStatComponent::InitializeComponent()
{
	Super::InitializeComponent();
	SetHP(MaxHP);
}

float UTACCharacterStatComponent::ApplyDamage(uint8 InDamage)
{
	const int8 PrevHP = CurrentHP;
	SetHP(PrevHP - InDamage);
	if(CurrentHP == 0)
	{
		OnHPZero.Broadcast();
	}
	return InDamage;
}

void UTACCharacterStatComponent::ReadyForReplication()
{
	Super::ReadyForReplication();
}

void UTACCharacterStatComponent::SetHP(int8 NewHP)
{
	CurrentHP = FMath::Clamp<int8>(NewHP, 0, MaxHP);
	OnHPChanged.Broadcast(CurrentHP);
}



void UTACCharacterStatComponent::OnRep_CurrentHP()
{
	OnHPChanged.Broadcast(CurrentHP);
	if(CurrentHP <= KINDA_SMALL_NUMBER)
	{
		OnHPZero.Broadcast();
	}
	TAC_SUBLOG(LogTACNetwork, Log, TEXT("%d"), CurrentHP);
}



