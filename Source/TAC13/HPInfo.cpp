 // Fill out your copyright notice in the Description page of Project Settings.


  #include "HPInfo.h"

UHPInfo::UHPInfo()
{
	
}

void UHPInfo::ChangeHPInfo(const float InCurrentHP, const float InMaxHP)
{
	OnChanged.Broadcast(InCurrentHP, InMaxHP);
}
