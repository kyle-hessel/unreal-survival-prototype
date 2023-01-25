// Fill out your copyright notice in the Description page of Project Settings.


#include "CampItemTrunk.h"

ACampItemTrunk::ACampItemTrunk()
{
	WorldItemIdentifier = 6; // Trunk
}

void ACampItemTrunk::Interact_Implementation(APawn* InstigatorPawn)
{
	Super::Interact_Implementation(InstigatorPawn);

	UE_LOG(LogTemp, Warning, TEXT("Trunk."));
}


