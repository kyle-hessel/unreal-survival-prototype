// Fill out your copyright notice in the Description page of Project Settings.


#include "CampAnimNotifyRagdoll.h"

// Ragdoll the given actor when this notify is triggered, presumably upon death or some other action requiring ragdolling.
void UCampAnimNotifyRagdoll::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	// Ragdoll the given mesh.
	MeshComp->SetAllBodiesSimulatePhysics(true);
	MeshComp->SetCollisionProfileName("Ragdoll");
}
