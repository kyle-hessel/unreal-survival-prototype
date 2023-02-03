// Fill out your copyright notice in the Description page of Project Settings.


#include "CampItemFirepit.h"

#include "CampCharacter.h"

ACampItemFirepit::ACampItemFirepit()
{
	WorldItemIdentifier = 7; // Firepit

	KindlingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("KindlingMesh"));
	KindlingMesh->SetupAttachment(Item);
}

void ACampItemFirepit::Interact_Implementation(APawn* InstigatorPawn)
{
	Super::Interact_Implementation(InstigatorPawn);

	UE_LOG(LogTemp, Warning, TEXT("Firepit."));

	if (ACampCharacter* CampCharacter = Cast<ACampCharacter>(InstigatorPawn))
	{
		if (CampCharacter->bInAccessBox == true)
		{
			
		}
	}
}
