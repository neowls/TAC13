
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
	TAC_SUBLOG(LogTACNetwork, Warning, TEXT("Start"));
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

	if(OnHPChanged.IsBound())
	{
		TAC_SUBLOG(LogTACNetwork, Warning, TEXT("OnHPChange Bounded."));
	}

}



void UTACCharacterStatComponent::OnRep_CurrentHP()
{
	TAC_SUBLOG(LogTACNetwork, Warning, TEXT("Start"));
	OnHPChanged.Broadcast(CurrentHP);
	if(CurrentHP <= KINDA_SMALL_NUMBER)
	{
		OnHPZero.Broadcast();
	}
}



