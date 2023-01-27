// Fill out your copyright notice in the Description page of Project Settings.


#include "CampItemTrunk.h"

#include "CampInventoryComponent.h"

ACampItemTrunk::ACampItemTrunk()
{
	WorldItemIdentifier = 6; // Trunk

	InventoryComp = CreateDefaultSubobject<UCampInventoryComponent>(TEXT("InventoryComp"));
}

void ACampItemTrunk::Interact_Implementation(APawn* InstigatorPawn)
{
	Super::Interact_Implementation(InstigatorPawn);

	UE_LOG(LogTemp, Warning, TEXT("Trunk."));

	
}


