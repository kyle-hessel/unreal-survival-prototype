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

	if (const ACampCharacter* CampCharacter = Cast<ACampCharacter>(InstigatorPawn))
	{
		if (CampCharacter->bInAccessBox == true)
		{
			
		}
	}
}

void ACampItemTent::PlaceItem_Implementation(FVector SpawnLocation, FRotator PlayerRotation)
{
	//Super::PlaceItem_Implementation(SpawnLocation, PlayerRotation)

	SetActorLocation(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z));
	const float RotationDifference = PlayerRotation.Yaw - GetActorRotation().Yaw;
	AddActorLocalRotation(FRotator(0.f, RotationDifference + 180.f, 0.f));
}
