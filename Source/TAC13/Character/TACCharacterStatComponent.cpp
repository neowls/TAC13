
#include "Character/TACCharacterStatComponent.h"


UTACCharacterStatComponent::UTACCharacterStatComponent()
{
	MaxHP = 100;
	bWantsInitializeComponent = true;
}



void UTACCharacterStatComponent::InitializeComponent()
{
	Super::InitializeComponent();
	SetHP(MaxHP);
}

float UTACCharacterStatComponent::ApplyDamage(uint8 InDamage)
{
	const uint8 PrevHP = CurrentHP;
	SetHP(PrevHP - InDamage);
	if(CurrentHP == 0)
	{
		OnHPZero.Broadcast();
	}
	return InDamage;
}

void UTACCharacterStatComponent::SetHP(uint8 NewHP)
{
	CurrentHP = FMath::Clamp<uint8>(NewHP, 0, MaxHP);
	OnHPChanged.Broadcast(CurrentHP);
}



