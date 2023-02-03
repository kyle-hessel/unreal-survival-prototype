// Fill out your copyright notice in the Description page of Project Settings.


#include "CampItemBench.h"

#include "CampCharacter.h"

ACampItemBench::ACampItemBench()
{
	WorldItemIdentifier = 4; // Bench
}

void ACampItemBench::Interact_Implementation(APawn* InstigatorPawn)
{
	Super::Interact_Implementation(InstigatorPawn);
	
	UE_LOG(LogTemp, Warning, TEXT("Bench."));

	if (ACampCharacter* CampCharacter = Cast<ACampCharacter>(InstigatorPawn))
	{
		if (CampCharacter->bInAccessBox == true)
		{
			if (CampCharacter->bSitting == false) CampCharacter->SitDown();
			else CampCharacter->StandUp();
		}
	}
}

void ACampItemBench::PlaceItem_Implementation(FVector SpawnLocation, FRotator PlayerRotation)
{
	Super::PlaceItem_Implementation(SpawnLocation, PlayerRotation);

	
}


