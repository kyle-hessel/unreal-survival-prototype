// Fill out your copyright notice in the Description page of Project Settings.


#include "CampItemBench.h"

#include "CampCharacter.h"
#include "CampInteractionComponent.h"

ACampItemBench::ACampItemBench()
{
	WorldItemIdentifier = 4; // Bench
}

void ACampItemBench::Interact_Implementation(APawn* InstigatorPawn)
{
	Super::Interact_Implementation(InstigatorPawn);

	if (ACampCharacter* CampCharacter = Cast<ACampCharacter>(InstigatorPawn))
	{
		if (CampCharacter->GetCampInteractComp()->GetCurrentUtilityItem() == this)
		{
			UE_LOG(LogTemp, Warning, TEXT("Bench."));
			
			if (CampCharacter->bInAccessBox == true)
			{
				if (CampCharacter->bSitting == false) CampCharacter->SitDown();
				else CampCharacter->StandUp();
			}
		}
	}
}

void ACampItemBench::OrientItem_Implementation(FRotator PlayerRotation)
{
	Super::OrientItem_Implementation(PlayerRotation);

	
}


