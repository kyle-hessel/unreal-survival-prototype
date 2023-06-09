// Fill out your copyright notice in the Description page of Project Settings.


#include "CampInteractionComponent.h"

#include "CampBackpack.h"
#include "CampCharacter.h"
#include "CampInteractionInterface.h"
#include "CampInventoryComponent.h"
#include "CampMeleeWeapon.h"
#include "CampWorldItem.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UCampInteractionComponent::UCampInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UCampInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	CampCharacter = Cast<ACampCharacter>(GetOwner());
	//PlayerController = Cast<APlayerController>(CampCharacter->GetController());
}


void UCampInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

// Triggered when interact key is pressed, checks for a collision with an interactable object and then decides what to do with said object.
void UCampInteractionComponent::PrimaryInteract()
{
	ACampWorldItem* TargetedItem = CampCharacter->GetTargetedItem();
	AACampEquipable* TargetedEquipable = CampCharacter->GetTargetedEquipable();
	APawn* OwningPawn = Cast<APawn>(GetOwner());

	// This will always prioritize targeted items, and will not pick up an equipable until all items in range are picked up. I'll see how this feels for now.
	if (TargetedItem)
	{
		if (OwningPawn) ICampInteractionInterface::Execute_Interact(TargetedItem, OwningPawn);
	}
	else if (TargetedEquipable)
	{
		// If the equipable is a backpack and the player isn't wearing one already, equip it.
		if (TargetedEquipable->IsA(ACampBackpack::StaticClass()) && HeldBackpack == nullptr)
		{
			HeldBackpack = Cast<ACampBackpack>(TargetedEquipable);
			// Close player inventory if backpack is equipped (this just sets the bool back, the below delegate actually closes it).
			CampCharacter->GetCampInventoryComp()->bIsOpen = false;

			// The below interface on equipables (in their respective classes) disables collision with WorldDynamic objects. This seems to trigger ...
			// ACampCharacter's EndItemSphereOverlap, which also removes them from the array of NearbyEquipables.
			if (OwningPawn) ICampInteractionInterface::Execute_Interact(TargetedEquipable, OwningPawn);
		}

		if (TargetedEquipable->IsA(ACampMeleeWeapon::StaticClass()) && CampCharacter->GetActiveMeleeWeapon() == nullptr)
		{
			if (OwningPawn) ICampInteractionInterface::Execute_Interact(TargetedEquipable, OwningPawn);
		}
	}
	// In this case the player isn't targeting anything.
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No targeted items or equipables in range."));
	}
}

// Called by CampCharacter when the Drop key is pressed.
void UCampInteractionComponent::DropEquipable()
{
	// If our HeldBackpack is currently occupied by something, then...
	if (HeldBackpack != nullptr)
	{
		// Close backpack inventory if backpack is dropped (this just sets the bool back, the below delegate actually closes it).
		HeldBackpack->GetCampInventoryComp()->bIsOpen = false;
		
		// If DropBackpack returned true meaning it was successful, then clear pointer to HeldBackpack.
		if (HeldBackpack->DropBackpack()) HeldBackpack = nullptr;
	}
}

// Check if we are holding the backpack or not without exposing our HeldBackpack pointer to other classes.
bool UCampInteractionComponent::HoldingBackpackOrNot() const
{
	const bool bHoldingBackpackOrNot = HeldBackpack != nullptr;
	
	return bHoldingBackpackOrNot;
}
