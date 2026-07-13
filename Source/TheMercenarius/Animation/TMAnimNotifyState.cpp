// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/TMAnimNotifyState.h"
#include "Character/TMPlayerBase.h"
#include "Weapon/TMWeaponBase.h"

void UTMAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		ATMPlayerBase* playerCharacter = Cast<ATMPlayerBase>(MeshComp->GetOwner());
		if (playerCharacter && playerCharacter->GetCurrentWeapon())
		{
			playerCharacter->GetCurrentWeapon()->SetWeaponCollision(true);
		}
	}
}

void UTMAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		ATMPlayerBase* playerCharacter = Cast<ATMPlayerBase>(MeshComp->GetOwner());
		if (playerCharacter && playerCharacter->GetCurrentWeapon())
		{
			playerCharacter->GetCurrentWeapon()->SetWeaponCollision(false);
		}
	}
}