// Fill out your copyright notice in the Description page of Project Settings.


#include "CampActionAttack.h"

#include "CampCharacter.h"

UCampActionAttack::UCampActionAttack()
{
	ActionName = TEXT("Attack");
}

void UCampActionAttack::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	if (ACampCharacter* CampCharacter = Cast<ACampCharacter>(Instigator))
	{
		CampCharacter->PlayAnimMontage(CampCharacter->DrawSwordAnimation);
	}
}

void UCampActionAttack::StopAction_Implementation(AActor* Instigator)
{
	if (ACampCharacter* CampCharacter = Cast<ACampCharacter>(Instigator))
	{
		CampCharacter->StopAnimMontage(CampCharacter->AttackChainAnimation);
	}

	Super::StopAction_Implementation(Instigator);
}


