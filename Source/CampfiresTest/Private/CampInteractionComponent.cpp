// Fill out your copyright notice in the Description page of Project Settings.


#include "CampInteractionComponent.h"

#include "CampBackpack.h"
#include "CampCampsite.h"
#include "CampCharacter.h"
#include "CampInteractionInterface.h"
#include "CampInventoryComponent.h"
#include "CampWorldItem.h"
#include "DrawDebugHelpers.h"
#include "Engine/StaticMeshSocket.h"

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
	// Set query parameters for collision trace
	FCollisionObjectQueryParams ObjectQueryParams;

	// Only look for equipables (isolated to GameTraceChannel1, check defaultengine.ini for clarity) if we don't have a backpack equipped.
	if (HeldBackpack == nullptr)
	{
		ObjectQueryParams.AddObjectTypesToQuery(ECC_GameTraceChannel1);
	}

	// Only look for melee weapons (isolated to GameTraceChannel2) if we don't have a melee weapon equipped.
	if (CampCharacter)
	{
		if (CampCharacter->GetActiveMeleeWeapon() == nullptr)
		{
			ObjectQueryParams.AddObjectTypesToQuery(ECC_GameTraceChannel2);
		}
	}

	// Always query for WorldStatic objects regardless.
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic); //***TEMPORARY?

	// Store our owning character's eye location & rotation - start of collision trace
	FVector EyeLocation;
	FRotator EyeRotation;
	GetOwner()->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	// Set end of line trace to the EyeLocation + EyeRotation converted to a directional vector, multiplied by an arbitrary amount to make it shoot forward.
	FVector EndLoc = EyeLocation + (EyeRotation.Vector() * 200);

	// Execute our collision trace and store the result in Hit.
	FHitResult Hit;
	
	//bool bBlockingHit = GetWorld()->LineTraceSingleByObjectType(Hit, EyeLocation, EndLoc, ObjectQueryParams);
	FCollisionShape SphereShape;
	SphereShape.SetSphere(30.0f);
	bool bBlockingHit = GetWorld()->SweepSingleByObjectType(Hit, EyeLocation, EndLoc, FQuat::Identity, ObjectQueryParams, SphereShape);

	// If we hit an actor, store that from the component we hit, or the component itself.
	HitActor = Hit.GetActor();
	HitComponent = Hit.GetComponent();

	if (HitActor) // If there is an owning actor on what we hit, which implies an owning component.
	{
		// If our HitActor implements our Interaction Interface, then...
		if (HitActor->Implements<UCampInteractionInterface>())
		{

			// If our HitActor is of type ACampBackpack and we aren't already holding one, store that in a variable we can use later.
			if (HeldBackpack == nullptr)
			{
				if (HitActor->IsA(ACampBackpack::StaticClass()))
				{
					HeldBackpack = Cast<ACampBackpack>(HitActor);
					UE_LOG(LogTemp, Warning, TEXT("Casted AActor to ACampBackpack."));

					// Close player inventory if backpack is equipped (this just sets the bool back, the below delegate actually closes it).
					CampCharacter->GetCampInventoryComp()->bIsOpen = false;
				}

				//if (Cast<ACampBackpack>(HitActor)) HeldBackpack = Cast<ACampBackpack>(HitActor); UE_LOG(LogTemp, Warning, TEXT("Casted AActor to ACampBackpack."));
			}

			// If our HitActor is of type CampCampsite, store that in a variable we can use later. Don't check for nullptr since our campsite can change on the fly.
			if (HitActor->IsA(ACampCampsite::StaticClass()))
			{
				CurrentCampsite = Cast<ACampCampsite>(HitActor);
				UE_LOG(LogTemp, Warning, TEXT("Casted AActor to ACampCampsite."));
			}

			// If we have stored a campsite
			if (CurrentCampsite && CampCharacter->bSitting == false)
			{
				// If we hit the bench within the campsite, sit in it as long as we aren't jumping.
				if (HitComponent->GetName() == "Bench" && CampCharacter->bJumping == false) { // **** Should I move some of this to CampCampsite? Might be more elegant.

					// Sit CampCharacter down on CampCampsite's Bench.
					CampCharacter->SitDown();
				}
			}
			// If we interact with the bench while sitting on it, stop sitting.
			else if (CurrentCampsite && CampCharacter->bSitting == true)
			{
				// Stand CampCharacter back up.
				CampCharacter->StandUp();
			}
			
			/* Lastly, call the interface's Interact function on the HitActor that we collided with during our trace,
			 * and pass in our owning Actor (casted to a Pawn) firing the trace as the Instigator.
			*/
			if (Cast<APawn>(GetOwner())) ICampInteractionInterface::Execute_Interact(HitActor, Cast<APawn>(GetOwner()));


			
			// If our HitActor is of type CampWorldItem . . . (do this after calling the interface so that the item is already added)
			if (HitActor->IsA(ACampWorldItem::StaticClass()))
			{
				TArray<FName> OutKeys;
				CampCharacter->GetCampInventoryComp()->GetInventoryContentsKeys(OutKeys);

				if (OutKeys.Num() > 0) UE_LOG(LogTemp, Display, TEXT("First item in inventory: %s"), *OutKeys[0].ToString());
			}
		}
		else
		{
			// Also stop sitting even if we hit something that doesn't implement CampActionInterface.
			if (CampCharacter->bSitting == true)
			{
				CampCharacter->StandUp();
			}
		}
	}
	else
	{
		// Also stop sitting even if we don't hit anything.
		if (CampCharacter->bSitting == true)
		{
			CampCharacter->StandUp();
		}
	}

	//Debug information
	FColor LineColor = bBlockingHit ? FColor:: Blue : FColor::Red;
	
	DrawDebugLine(GetWorld(), EyeLocation, EndLoc, LineColor, false, 3.0f, 0, 2.0f);
	DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 30.0f, 32, LineColor, false, 3.0f, 0, 1.5f);
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
