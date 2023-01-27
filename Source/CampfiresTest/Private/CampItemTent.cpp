// Fill out your copyright notice in the Description page of Project Settings.


#include "CampItemTent.h"

#include "CampCharacter.h"

ACampItemTent::ACampItemTent()
{
	WorldItemIdentifier = 5; // Tent
}

void ACampItemTent::Interact_Implementation(APawn* InstigatorPawn)
{
	Super::Interact_Implementation(InstigatorPawn);

	UE_LOG(LogTemp, Warning, TEXT("Tent."));

	if (ACampCharacter* CampCharacter = Cast<ACampCharacter>(InstigatorPawn))
	{
		if (CampCharacter->bInAccessBox == true)
		{
			
		}
	}
}
