// Fill out your copyright notice in the Description page of Project Settings.


#include "CampItemTent.h"

#include "CampCharacter.h"
#include "CampInteractionComponent.h"

ACampItemTent::ACampItemTent()
{
	WorldItemIdentifier = 5; // Tent
}

void ACampItemTent::Interact_Implementation(APawn* InstigatorPawn)
{
	Super::Interact_Implementation(InstigatorPawn);
	

	if (const ACampCharacter* CampCharacter = Cast<ACampCharacter>(InstigatorPawn))
	{
		if (CampCharacter->GetCampInteractComp()->GetCurrentUtilityItem() == this)
		{
			UE_LOG(LogTemp, Warning, TEXT("Tent."));
			
			if (CampCharacter->bInAccessBox == true)
			{
			
			}
		}
	}
}

void ACampItemTent::OrientItem_Implementation(FRotator PlayerRotation)
{
	Super::OrientItem_Implementation(PlayerRotation);
}
