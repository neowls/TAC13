// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_ChangeWeaponCheck.h"
#include "Interface/TACAnimationWeaponInterface.h"

void UAnimNotify_ChangeWeaponCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if(MeshComp)
	{
		ITACAnimationWeaponInterface* WeaponPawn = Cast<ITACAnimationWeaponInterface>(MeshComp->GetOwner());
		if(WeaponPawn)
		{
			WeaponPawn->ChangeWeaponCheck();
		}
	}
}
