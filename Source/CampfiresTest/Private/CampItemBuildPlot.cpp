// Fill out your copyright notice in the Description page of Project Settings.


#include "CampItemBuildPlot.h"

#include "CampBackpack.h"
#include "CampCharacter.h"
#include "CampGameModeBase.h"
#include "CampInteractionComponent.h"
#include "CampInventoryComponent.h"
#include "CampWorldItem.h"
#include "MyCampWorldUtilityItem.h"
#include "Components/BoxComponent.h"

class ACampWorldItem;

ACampItemBuildPlot::ACampItemBuildPlot()
{
	PrimaryActorTick.bCanEverTick = false;

	BoundingBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoundingBox"));
	BoundingBox->SetCollisionResponseToAllChannels(ECR_Overlap);
	SetRootComponent(BoundingBox);

	bBuildingItem = false;

	BoundingBox->OnComponentBeginOverlap.AddDynamic(this, &ACampItemBuildPlot::BeginBoxOverlap);
	BoundingBox->OnComponentEndOverlap.AddDynamic(this, &ACampItemBuildPlot::EndBoxOverlap);
}

void ACampItemBuildPlot::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACampItemBuildPlot::BeginBoxOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(ACampWorldItem::StaticClass()))
	{
		ACampWorldItem* DroppedWorldItem = Cast<ACampWorldItem>(OtherActor);
		OverlappingItems.Add(DroppedWorldItem);
		UE_LOG(LogTemp, Warning, TEXT("Item detected."));
	}
}

void ACampItemBuildPlot::EndBoxOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// If an item ends box overlap, remove it from overlapping items to avoid required resource duplication.
	// A bool check for bBuildingItem (set to true below) is also needed here so that this only fires when an item exits when we are NOT in the process of building.
	if (OtherActor->IsA(ACampWorldItem::StaticClass()) && bBuildingItem == false)
	{
		ACampWorldItem* PickedUpWorldItem = Cast<ACampWorldItem>(OtherActor);
		OverlappingItems.Remove(PickedUpWorldItem);
		UE_LOG(LogTemp, Warning, TEXT("Overlapping item removed."));
	}

	// If the character is leaving the plot, see if we can spawn the build, and if we can, do so (CalculateAcquiredMaterials()).
	if (OtherActor->IsA(ACampCharacter::StaticClass()))
	{
		CampCharacter = Cast<ACampCharacter>(OtherActor);

		bBuildingItem = true;
		
		CalculateAcquiredMaterials();
		UE_LOG(LogTemp, Warning, TEXT("Character leaving."));
	}
}

void ACampItemBuildPlot::CalculateAcquiredMaterials()
{
	TMap<FName, int32> ItemsAllocated;
	TArray<TObjectPtr<ACampWorldItem>> ItemsPendingDeletion;
	
	for (ACampWorldItem* Item : OverlappingItems)
	{
		// Only add items we need to our temporary list.
		if (const int32* ItemValPtr = ItemsRequired.Find(Item->WorldItemName))
		{
			const int32* ItemValPtrTemp = ItemsAllocated.Find(Item->WorldItemName);
			// If we already have an existing quantity of the given item, then...
			if (ItemValPtrTemp)
			{
				// Only increment the temp quantity if it is less than the necessary amount.
				if (*ItemValPtrTemp < *ItemValPtr)
				{
					ItemsAllocated.Add(Item->WorldItemName, *ItemValPtrTemp + 1);
					ItemsPendingDeletion.Add(Item);
				}
			}
			// If we do not yet have the given item in our ItemsRequiredTemp map but we have a match, just add 1 of it.
			else
			{
				ItemsAllocated.Add(Item->WorldItemName, 1);
				ItemsPendingDeletion.Add(Item);
			}
		}
	}

	// Compare the ItemsRequiredTemp Map against the ItemsAllocated Map. If they are a perfect match, then we have everything needed to build.
	if (ItemsRequired.OrderIndependentCompareEqual(ItemsAllocated)) // where has this magical function been all my life
	{
		UE_LOG(LogTemp, Warning, TEXT("Beginning consumption of resources."));

		// Just before construction, delete all items that were allocated for the build from the ground.
		for (ACampWorldItem* Deleting : ItemsPendingDeletion)
		{
			// Remove any item we are about to delete from our OverlappingItems array.
			OverlappingItems.Remove(Deleting);

			// Destroy this item as it was marked for consumption, and set it to a nullptr in the array itself.
			Deleting->Destroy();
			Deleting = nullptr;
		}

		ItemsPendingDeletion.Empty();
		
		SpawnBuildAndDeleteSelf();
	}
}


void ACampItemBuildPlot::SpawnBuildAndDeleteSelf()
{
	bool bAdded;
	
	// Return extra overlapping items back to the player's inventory before destroying the plot.
	// Do this before spawning the build item, or it will be duplicated as it will be considered overlapping on this frame.
	for (ACampWorldItem* Item : OverlappingItems)
	{
		// If wearing a backpack, default to adding items to that.
		if (const ACampBackpack* Backpack = CampCharacter->GetCampInteractComp()->GetHeldBackpack())
		{
			bAdded = Backpack->GetCampInventoryComp()->AddItemToInventory(Item->WorldItemName, 1, Item->bIsStackable);
		}
		// Otherwise, add it to the player's pockets.
		else
		{
			// Add item to inventory map.
			bAdded = CampCharacter->GetCampInventoryComp()->AddItemToInventory(Item->WorldItemName, 1, Item->bIsStackable);
		}

		// Only destroy the item if it was successfully added to the inventory. If not, just clear the pointer but leave it on the ground.
		if (bAdded) Item->Destroy();
		Item = nullptr;
	}

	FActorSpawnParameters SpawnParams;

	const FTransform SpawnTransform = FTransform(GetActorRotation(), GetActorLocation());
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	TArray<FName> RowNames = Items->GetRowNames();
	ACampWorldItem* SpawnedItem;

	// Determine which subclass of camp world utility item to spawn, and if item isn't one, spawn a normal camp world item.
	// Using TSubClassOf variables here that are set in the editor so that functionality from both C++ and blueprint extended versions of the below classes is used.
	if (RowNames[BuildItemIdentifier] == FName(TEXT("Firepit"))) // Sadly can't do a switch statement on FNames, apparently. If converted to an FString, I could.
	{
		SpawnedItem = GetWorld()->SpawnActor<AMyCampWorldUtilityItem>(FirepitClass, SpawnTransform, SpawnParams);
	}
	else if (RowNames[BuildItemIdentifier] == FName(TEXT("Tent")))
	{
		SpawnedItem = GetWorld()->SpawnActor<AMyCampWorldUtilityItem>(TentClass, SpawnTransform, SpawnParams);
	}
	else if (RowNames[BuildItemIdentifier] == FName(TEXT("Trunk")))
	{
		SpawnedItem = GetWorld()->SpawnActor<AMyCampWorldUtilityItem>(TrunkClass, SpawnTransform, SpawnParams);
	}
	else if (RowNames[BuildItemIdentifier] == FName(TEXT("Bench")))
	{
		SpawnedItem = GetWorld()->SpawnActor<AMyCampWorldUtilityItem>(BenchClass, SpawnTransform, SpawnParams);
	}
	else
	{
		SpawnedItem = GetWorld()->SpawnActor<ACampWorldItem>(ACampWorldItem::StaticClass(), SpawnTransform, SpawnParams);
	}
	
	const FItemStruct* NewItemData = Items->FindRow<FItemStruct>(RowNames[BuildItemIdentifier], "Create", true);
	
	SpawnedItem->WorldItemIdentifier = BuildItemIdentifier;
	SpawnedItem->WorldItemName = RowNames[BuildItemIdentifier];
	SpawnedItem->DisplayName = NewItemData->DisplayName;
	SpawnedItem->DisplayDescription = NewItemData->DisplayDescription;
	SpawnedItem->bIsStackable = NewItemData->bIsStackable;
	SpawnedItem->Item->SetStaticMesh(NewItemData->Mesh);
	SpawnedItem->ItemType = NewItemData->ItemType;
	SpawnedItem->bEnergyItem = NewItemData->bEnergyItem;
	SpawnedItem->EnergyDelta = NewItemData->EnergyDelta;
	SpawnedItem->bLifeForceItem = NewItemData->bLifeForceItem;
	SpawnedItem->LifeForceDelta = NewItemData->LifeForceDelta;
	SpawnedItem->bCraftable = NewItemData->bCraftable; 
	if (SpawnedItem->bCraftable) SpawnedItem->IngredientsToCraft = NewItemData->IngredientsToCraft;

	UE_LOG(LogTemp, Warning, TEXT("Spawned new item."));

	CampCharacter->BuildSiteLocations.Remove(GetActorLocation());

	CampCharacter = nullptr;

	OverlappingItems.Empty();

	// Delete the plot (this object) now that every task is complete.
	this->Destroy();
}

