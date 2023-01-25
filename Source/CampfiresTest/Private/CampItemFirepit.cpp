// Fill out your copyright notice in the Description page of Project Settings.


#include "CampItemFirepit.h"

ACampItemFirepit::ACampItemFirepit()
{
	WorldItemIdentifier = 7; // Firepit
}

void ACampItemFirepit::Interact_Implementation(APawn* InstigatorPawn)
{
	Super::Interact_Implementation(InstigatorPawn);

	UE_LOG(LogTemp, Warning, TEXT("Firepit."));
}
