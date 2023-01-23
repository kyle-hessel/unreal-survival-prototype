// Fill out your copyright notice in the Description page of Project Settings.


#include "CampAnimNotifyMeleeToggle.h"

#include "CampCharacter.h"
#include "CampMeleeWeapon.h"
#include "Engine/SkeletalMeshSocket.h"

void UCampAnimNotifyMeleeToggle::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                        const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	// Swap sword from holster to in-hand by swapping sockets mid-animation. This class is triggered in the draw sword animation.
	if (ACampCharacter* CampCharacter = Cast<ACampCharacter>(MeshComp->GetOwner()))
	{
		if (ACampMeleeWeapon* CampCharacterMeleeWeapon = CampCharacter->GetActiveMeleeWeapon())
		{
			// If we just entered combat, attach sword to hand from waist.
			if (CampCharacterMeleeWeapon->bInCombat == true)
			{
				UE_LOG(LogTemp, Warning, TEXT("Relocated melee weapon to new socket."));
				CampCharacterMeleeWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
				CampCharacterMeleeWeapon->CombatSocket->AttachActor(CampCharacterMeleeWeapon, CampCharacter->GetMesh());

				CampCharacter->bWeaponDrawn = true;
			}
			// If we just left combat, attach sword to waist from hand.
			else
			{
				CampCharacterMeleeWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
				CampCharacterMeleeWeapon->HolsterSocket->AttachActor(CampCharacterMeleeWeapon, CampCharacter->GetMesh());

				CampCharacter->bWeaponDrawn = false;
			}
		}
	}
}
