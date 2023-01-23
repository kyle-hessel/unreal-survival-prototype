// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "CampAnimNotifyMeleeToggle.generated.h"

class ACampCharacter;

/**
 * 
 */
UCLASS()
class CAMPFIRESTEST_API UCampAnimNotifyMeleeToggle : public UAnimNotify
{
	GENERATED_BODY()

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
};
