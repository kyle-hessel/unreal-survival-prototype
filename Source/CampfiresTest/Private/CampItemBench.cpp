// Fill out your copyright notice in the Description page of Project Settings.


#include "CampItemBench.h"

ACampItemBench::ACampItemBench()
{
	WorldItemIdentifier = 4; // Bench
}

void ACampItemBench::Interact_Implementation(APawn* InstigatorPawn)
{
	Super::Interact_Implementation(InstigatorPawn);
	
	UE_LOG(LogTemp, Warning, TEXT("Bench."));
}


