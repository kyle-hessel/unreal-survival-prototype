// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CampAction.h"
#include "CampActionAttack.generated.h"

class ACampCharacter;
class UAnimMontage;

/**
 * 
 */
UCLASS()
class CAMPFIRESTEST_API UCampActionAttack : public UCampAction
{
	GENERATED_BODY()

public:
	
	UCampActionAttack();

protected:

	void StartAction_Implementation(AActor* Instigator) override;
	void StopAction_Implementation(AActor* Instigator) override;
	
};
