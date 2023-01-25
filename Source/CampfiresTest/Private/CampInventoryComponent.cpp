// Fill out your copyright notice in the Description page of Project Settings.


#include "CampInventoryComponent.h"

#include "CampAttributeComponent.h"
#include "CampBackpack.h"
#include "CampCampsite.h"
#include "CampCharacter.h"
#include "CampGameModeBase.h"
#include "CampWorldItem.h"
#include "Kismet/KismetMathLibrary.h"

UCampInventoryComponent::UCampInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	InventoryName = TEXT("Default");
	
	ItemIncrement = 0;

	bIsOpen = false;

	MaxSize = 2;
}

void UCampInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UCampInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
}

// Add an item to the given inventory using TMap.
bool UCampInventoryComponent::AddItemToInventory(FName ItemName, int32 Quantity, bool bStackable)
{
	// Check if the inventory already has a given quantity of the existing item and store this value in a pointer.
	int32* ExistingItemQuantity = InventoryContents.Find(ItemName);
	
	if (ExistingItemQuantity != nullptr)
	{
		if (bStackable == true)
		{
			// Get value stored in ExistingItemQuantity and add it to the quantity of the given item.
			const int32 NewQuantity = *ExistingItemQuantity + Quantity;
		
			// Update existing key-value pairing of a given item with the new quantity of said item.
			InventoryContents.Add(ItemName, NewQuantity);
			// Broadcast a delegate so that we can also refresh inventory screens in blueprint when picking up items. Might want to add a check here to not do this on campsites, maybe?
			OnItemPickup.Broadcast();

			UE_LOG(LogTemp, Display, TEXT("Added item: %s to inventory!"), *ItemName.ToString());

			return true;
		}
	}
	// If we do not yet have the given item (nullptr quantity), create a key-value pairing for it.
	else
	{
		// Only check if inventory is full for non-stackable items.
		if (InventoryContents.Num() < MaxSize)
		{
			// If the item isn't stackable, add a value to it and append it to its name to give it a unique key as there may already technically be duplicates in the inventory that aren't stacked.
			if (bStackable == false)
			{
				FString ItemNameConverted = ItemName.ToString();
			
				// Generate a different random(ish) key by adding a number to the end of ItemName.
				ItemIncrement++;
				FString ConvertedCount = FString::FromInt(ItemIncrement);

				// Append a number to the end of the name so that this new item of the same type has a unique key.
				ItemNameConverted.Append(ConvertedCount);
				FName NewItemName = FName(*ItemNameConverted);
			
				InventoryContents.Add(NewItemName, Quantity);
				OnItemPickup.Broadcast();

				UE_LOG(LogTemp, Display, TEXT("Added item: %s to inventory!"), *ItemName.ToString());

				return true;
			}
			// If the item is stackable, add it normally.
			else
			{
				// Add new item with its quantity in a key-value pair.
				InventoryContents.Add(ItemName, Quantity);
				OnItemPickup.Broadcast();

				UE_LOG(LogTemp, Display, TEXT("Added item: %s to inventory!"), *ItemName.ToString());

				return true;
			}
		}
	}

	// Return false if we did not successfully add an item.
	return false;
}

// Remove an item from the given inventory using TMap.
bool UCampInventoryComponent::RemoveItemFromInventory(FName ItemNameAndNumber, FName ItemName, int32 Quantity)
{
	// Check if the inventory already has a given quantity of the existing item and store this value in a pointer.
	const int32* ExistingItemCount = InventoryContents.Find(ItemNameAndNumber);

	bool bDroppedOrNot = false;
	
	// If the item has a quantity larger than one, decrement the quantity on the existing item.
	if (*ExistingItemCount > 1)
	{
		// If we are dropping multiple of this item, do that.
		if (Quantity > 1)
		{
			int32 QuantityIncrement = 0;
			
			// Spawn item as many times as necessary when dropping multiple.
			while (QuantityIncrement < Quantity)
			{
				
				if (SpawnItem(ItemName)) bDroppedOrNot = true;
				QuantityIncrement++;
			}
		}
		// If we are only dropping one, do that.
		else
		{
			// Spawn the item before removing it from inventory. If the item is unable to spawn due to uneven terrain, don't do anything.
			if (SpawnItem(ItemName)) bDroppedOrNot = true;
		}
		
		if (bDroppedOrNot)
		{
			const int32 NewQuantity = *ExistingItemCount - Quantity;

			if (NewQuantity > 0)
			{
				// Update existing key-value pairing of a given item with the new quantity of said item.
				InventoryContents.Add(ItemNameAndNumber, NewQuantity);
				// Broadcast a delegate so that we can also refresh inventory screens in blueprint when dropping items. Might want to add a check here to not do this on campsites, maybe?
				// Might need to change this to its own delegate too if I add extra functionality to item pickups in blueprint besides just inventory refreshing.
				OnItemPickup.Broadcast();

				UE_LOG(LogTemp, Display, TEXT("Decremented given item's quantity in inventory."));

				// Return false because we are only removing some of this item.
				return false;
			}
			// If resulting quantity is 0, remove from inventory entirely.
			else
			{
				
				InventoryContents.Remove(ItemNameAndNumber);

				UE_LOG(LogTemp, Display, TEXT("Removed item: %s from inventory!"), *ItemName.ToString());

				return true;
			}
		}
	}
	// Otherwise, completely remove the item from the inventory.
	else
	{
		if (SpawnItem(ItemName))
		{
			InventoryContents.Remove(ItemNameAndNumber);

			UE_LOG(LogTemp, Display, TEXT("Removed item: %s from inventory!"), *ItemName.ToString());
			// Don't refresh inventory with OnItemPickup delegate: this is done in blueprint after this function is called if the item is completely removed.

			return true;
		}
	}

	// Return false if nothing else evaluated already.
	return false;
	
}

// Works the same as RemoveItemFromInventory, but doesn't spawn an item by default. Used for dragging items around between inventories, dropping onto ground, or consuming.
// Think of consuming as dragging into the player's mouth, or into creation of a structure.
bool UCampInventoryComponent::DragItemOutOfInventory(FName ItemNameAndNumber, FName ItemName, int32 Quantity)
{
	// Check if the inventory already has a given quantity of the existing item and store this value in a pointer.
	const int32* ExistingItemCount = InventoryContents.Find(ItemNameAndNumber);

	// If the item has a quantity larger than one, decrement the quantity on the existing item.
	if (*ExistingItemCount > 1)
	{
		const int32 NewQuantity = *ExistingItemCount - Quantity;

		if (NewQuantity > 0)
		{
			// Update existing key-value pairing of a given item with the new quantity of said item.
			InventoryContents.Add(ItemNameAndNumber, NewQuantity);
			// Broadcast a delegate so that we can also refresh inventory screens in blueprint when dropping items. Might want to add a check here to not do this on campsites, maybe?
			// Might need to change this to its own delegate too if I add extra functionality to item pickups in blueprint besides just inventory refreshing.
			OnItemPickup.Broadcast();

			UE_LOG(LogTemp, Display, TEXT("Decremented given item's quantity in inventory."));

			// Return false because we are only removing some of this item.
			return false;
		}
		// If quantity is going to equal 0, completely remove item.
		else
		{
			InventoryContents.Remove(ItemNameAndNumber);

			UE_LOG(LogTemp, Display, TEXT("Removed item: %s from inventory!"), *ItemName.ToString());

			return true;
		}

	}
	// Otherwise, completely remove the item from the inventory.
	else
	{
		InventoryContents.Remove(ItemNameAndNumber);

		UE_LOG(LogTemp, Display, TEXT("Removed item: %s from inventory!"), *ItemName.ToString());
		// Don't refresh inventory with OnItemPickup delegate: this is done in blueprint after this function is called if the item is completely removed.

		return true;
	}

	// Return false if nothing else evaluated already.
	return false;
}

// Spawns a given item on the ground. Usually used when dropping items from inventory.
// ** NOTE: don't mark this const. Makes it a blueprint pure function for some reason (in certain classes like BP_DragDrop), breaking functionality.
bool UCampInventoryComponent::SpawnItem(FName ItemName)
{
	// *** WILL need heavy refactoring

	// Do a line trace downwards to determine if the item can be dropped.
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);

	float ActorLocZOffset = GetOwner()->GetActorLocation().Z - 150.f;

	// This could be improved a bit to also factor in actor rotation to determine line trace positioning, just haven't found the right approach yet.
	FHitResult Hit;
	FVector LineTraceStart = GetOwner()->GetActorLocation(); // These values will need tweaking depending on character size...
	FVector LineTraceEnd = FVector(GetOwner()->GetActorLocation().X, GetOwner()->GetActorLocation().Y, ActorLocZOffset);

	bool bBlockingHit = GetWorld()->LineTraceSingleByObjectType(Hit, LineTraceStart, LineTraceEnd, ObjectQueryParams);
	DrawDebugLine(GetWorld(), LineTraceStart, LineTraceEnd, FColor::Silver, false, 3.0f, 0, 1.5f);

	// Only drop the item if line trace succeeds
	if (bBlockingHit)
	{
		FRotator GroundAlignedRotation = UKismetMathLibrary::MakeRotFromZX(Hit.ImpactNormal, GetOwner()->GetActorForwardVector());
		
		FTransform SpawnTransform = FTransform(GroundAlignedRotation, Hit.Location);
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		// Spawn the item back on the ground.
		// Need to add a switch statement here for item names; a few specific items should spawn their special class type, and everything else can spawn ACampWorldItem.
		// Everything below these two lines should be fine.
		ACampWorldItem* SpawnedItem;
		SpawnedItem = GetWorld()->SpawnActor<ACampWorldItem>(ACampWorldItem::StaticClass(), SpawnTransform, SpawnParams);

		// Once the item is spawned, repopulate it with the proper information from the row corresponding to its name.
		FItemStruct* NewItemData = Items->FindRow<FItemStruct>(ItemName, "Create", true);
		
		SpawnedItem->WorldItemName = ItemName;
		SpawnedItem->DisplayName = NewItemData->DisplayName;
		SpawnedItem->DisplayDescription = NewItemData->DisplayDescription;
		SpawnedItem->bIsStackable = NewItemData->bIsStackable;
		SpawnedItem->Item->SetStaticMesh(NewItemData->Mesh);
		SpawnedItem->Item->SetWorldScale3D(FVector(0.33f, 0.33f, 0.33f)); //*** TEMPORARY
		// SpawnedItem->SetMaterial() // This or SetMaterialByName will likely be necessary later.

		// Adjust the height of the mesh so it doesn't go into the ground.
		// Pull some logic here that is similar to how CampCampsite originally created campsites: check nearby landscape relative to bounding box size, and change the item's angle on terrain.
		FVector OriginalActorLocation = SpawnedItem->GetActorLocation();
		FVector HeightAdjustment = SpawnedItem->Item->GetStaticMesh()->GetBoundingBox().GetSize();

		SpawnedItem->SetActorLocation(FVector(OriginalActorLocation.X, OriginalActorLocation.Y, OriginalActorLocation.Z + HeightAdjustment.Z * 0.5f * 0.33f)); //*** 0.33F IS TEMPORARY

		return true;
	}

	return false;
}

// Consume an item—typically eating it to restore energy or life force. Note: Doesn't remove from inventory. Use DragItemOutOfInventory/RemoveItemFromInventory
// *** Refactor later to account for EItemType as well - refer to plans in GDD.
void UCampInventoryComponent::ConsumeItem(FName ItemNameAndNumber, const FName ItemName, int32 Quantity) const
{
	// Figure out what item we're consuming.
	const FItemStruct* ItemData = Items->FindRow<FItemStruct>(ItemName, "Consume", true);

	// If item is an energy item, determine how much energy to give to the player. Player is called in different ways depending on what inventory we are in.
	if (ItemData->bEnergyItem == true)
	{
		if (GetOwner()->IsA(ACampBackpack::StaticClass()))
		{
			const ACampBackpack* Backpack = Cast<ACampBackpack>(GetOwner());
			const ACampCharacter* CampCharacter = Backpack->GetOwningCharacter();

			if (UCampAttributeComponent* AttributeComp = CampCharacter->GetCampAttributeComp())
			{
				AttributeComp->ApplyEnergyChange(ItemData->EnergyDelta);
				UE_LOG(LogTemp, Warning, TEXT("Energy gained!"));
			}
		}
		else if (GetOwner()->IsA(ACampCharacter::StaticClass()))
		{
			const ACampCharacter* CampCharacter = Cast<ACampCharacter>(GetOwner());

			if (UCampAttributeComponent* AttributeComp = CampCharacter->GetCampAttributeComp())
			{
				AttributeComp->ApplyEnergyChange(ItemData->EnergyDelta);
				UE_LOG(LogTemp, Warning, TEXT("Energy gained!"));
			}
		}
		else if (GetOwner()->IsA(ACampCampsite::StaticClass()))
		{
			const ACampCampsite* Campsite = Cast<ACampCampsite>(GetOwner());
			const ACampCharacter* CampCharacter = Campsite->GetOwningCharacter();

			if (UCampAttributeComponent* AttributeComp = CampCharacter->GetCampAttributeComp())
			{
				AttributeComp->ApplyEnergyChange(ItemData->EnergyDelta);
				UE_LOG(LogTemp, Warning, TEXT("Energy gained!"));
			}
		}
	}

	// If item is a life force item, determine how much life force to give to the player. Player is called in different ways depending on what inventory we are in.
	if (ItemData->bLifeForceItem == true)
	{
		if (GetOwner()->IsA(ACampBackpack::StaticClass()))
		{
			const ACampBackpack* Backpack = Cast<ACampBackpack>(GetOwner());
			const ACampCharacter* CampCharacter = Backpack->GetOwningCharacter();

			if (UCampAttributeComponent* AttributeComp = CampCharacter->GetCampAttributeComp())
			{
				AttributeComp->ApplyLifeForceChange(ItemData->LifeForceDelta);
				UE_LOG(LogTemp, Warning, TEXT("Health gained!"));
			}
		}
		else if (GetOwner()->IsA(ACampCharacter::StaticClass()))
		{
			const ACampCharacter* CampCharacter = Cast<ACampCharacter>(GetOwner());

			if (UCampAttributeComponent* AttributeComp = CampCharacter->GetCampAttributeComp())
			{
				AttributeComp->ApplyLifeForceChange(ItemData->LifeForceDelta);
				UE_LOG(LogTemp, Warning, TEXT("Health gained!"));
			}
		}
		else if (GetOwner()->IsA(ACampCampsite::StaticClass()))
		{
			const ACampCampsite* Campsite = Cast<ACampCampsite>(GetOwner());
			const ACampCharacter* CampCharacter = Campsite->GetOwningCharacter();

			if (UCampAttributeComponent* AttributeComp = CampCharacter->GetCampAttributeComp())
			{
				AttributeComp->ApplyLifeForceChange(ItemData->LifeForceDelta);
				UE_LOG(LogTemp, Warning, TEXT("Health gained!"));
			}
		}
	}
}
