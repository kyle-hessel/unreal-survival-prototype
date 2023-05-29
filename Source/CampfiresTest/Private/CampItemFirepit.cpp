// Fill out your copyright notice in the Description page of Project Settings.


#include "CampItemFirepit.h"
#include "NiagaraComponent.h"

#include "CampCharacter.h"
#include "CampInteractionComponent.h"

ACampItemFirepit::ACampItemFirepit()
{
	WorldItemIdentifier = 7; // Firepit
	bLit = false;

	KindlingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("KindlingMesh"));
	KindlingMesh->SetupAttachment(Item);

	CampfireSystem = CreateDefaultSubobject<UNiagaraComponent>(TEXT("CampfireSystem"));
	CampfireSystem->SetupAttachment(KindlingMesh);
	CampfireSystem->SetAutoActivate(false);
}

void ACampItemFirepit::Interact_Implementation(APawn* InstigatorPawn)
{
	Super::Interact_Implementation(InstigatorPawn);

	if (const ACampCharacter* CampCharacter = Cast<ACampCharacter>(InstigatorPawn))
	{
		if (CampCharacter->GetCampInteractComp()->GetCurrentUtilityItem() == this)
		{
			UE_LOG(LogTemp, Warning, TEXT("Firepit."));
			
			if (bBeingAccessed && !bLit)
			{
				CampfireSystem->ActivateSystem();
				bLit = true;
			}
		}
	}
}

void ACampItemFirepit::OrientItem_Implementation(FRotator PlayerRotation)
{
	Super::OrientItem_Implementation(PlayerRotation);
}
