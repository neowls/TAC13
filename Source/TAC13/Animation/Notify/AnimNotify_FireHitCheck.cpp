// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notify/AnimNotify_FireHitCheck.h"
#include "Interface/TACAnimationWeaponInterface.h"

void UAnimNotify_FireHitCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if(MeshComp)
	{
		ITACAnimationWeaponInterface* FiredPawn = Cast<ITACAnimationWeaponInterface>(MeshComp->GetOwner());
		if(FiredPawn)
		{
			FiredPawn->FireHitCheck();
		}
	}
}
