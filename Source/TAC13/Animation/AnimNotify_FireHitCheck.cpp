// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_FireHitCheck.h"
#include "Interface/TACAnimationFireInterface.h"

void UAnimNotify_FireHitCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if(MeshComp)
	{
		ITACAnimationFireInterface* FiredPawn = Cast<ITACAnimationFireInterface>(MeshComp->GetOwner());
		if(FiredPawn)
		{
			FiredPawn->FireHitCheck();
		}
	}
}
