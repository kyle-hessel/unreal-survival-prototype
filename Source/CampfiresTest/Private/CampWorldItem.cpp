// Fill out your copyright notice in the Description page of Project Settings.


#include "CampWorldItem.h"

#include "CampBackpack.h"
#include "CampCharacter.h"
#include "CampInteractionComponent.h"
#include "CampInventoryComponent.h"
#include "CampGameModeBase.h"

ACampWorldItem::ACampWorldItem()
{
	PrimaryActorTick.bCanEverTick = true;

	Item = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Item"));
	Item->SetCollisionObjectType(ECC_WorldStatic);
	Item->SetCollisionResponseToAllChannels(ECR_Overlap);
	//Item->SetCollisionResponseToChannel()
	SetRootComponent(Item);

	// Set world item defaults, change these per-item in blueprint as necessary.
	bIsHoldable = true;

	ItemType = EItemType::ITEM_FoodRaw;
	bEnergyItem = false;
	EnergyDelta = 0.0f;
	bLifeForceItem = false;
	LifeForceDelta = 0.0f;
	bCraftable = false;

	// This determines which row in our data table we use, which determines which item this will be! Set in blueprint or defaults to 0.
	WorldItemIdentifier = 0;
}

void ACampWorldItem::BeginPlay()
{
	Super::BeginPlay();

	// I think I can just refactor this to use WorldItemName instead of this array and WorldItemIdentifier, just have a default value for the former and use that to find the row.
	
	// Set item parameters fetched from the Items data table.
	TArray<FName> RowNames = Items->GetRowNames();
	WorldItemName = RowNames[WorldItemIdentifier]; // For actual item identification when adding to inventory.
	DisplayName = Items->FindRow<FItemStruct>(RowNames[WorldItemIdentifier], "Find", true)->DisplayName; // For UI
	DisplayDescription = Items->FindRow<FItemStruct>(RowNames[WorldItemIdentifier], "Find", true)->DisplayDescription; // For UI
	bIsStackable = Items->FindRow<FItemStruct>(RowNames[WorldItemIdentifier], "Find", true)->bIsStackable;
	Item->SetStaticMesh(Items->FindRow<FItemStruct>(RowNames[WorldItemIdentifier], "Find", true)->Mesh);
	// Item->SetMaterial() // This or SetMaterialByName will likely be necessary later.
	ItemType = Items->FindRow<FItemStruct>(RowNames[WorldItemIdentifier], "Find", true)->ItemType;
	bEnergyItem = Items->FindRow<FItemStruct>(RowNames[WorldItemIdentifier], "Find", true)->bEnergyItem;
	EnergyDelta = Items->FindRow<FItemStruct>(RowNames[WorldItemIdentifier], "Find", true)->EnergyDelta;
	bLifeForceItem = Items->FindRow<FItemStruct>(RowNames[WorldItemIdentifier], "Find", true)->bLifeForceItem;
	LifeForceDelta = Items->FindRow<FItemStruct>(RowNames[WorldItemIdentifier], "Find", true)->LifeForceDelta;
	bCraftable = Items->FindRow<FItemStruct>(RowNames[WorldItemIdentifier], "Find", true)->bCraftable;
	if (bCraftable) IngredientsToCraft = Items->FindRow<FItemStruct>(RowNames[WorldItemIdentifier], "Find", true)->IngredientsToCraft;
}


void ACampWorldItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Triggered when the player interacts with the item through CampInteractionComponent.
void ACampWorldItem::Interact_Implementation(APawn* InstigatorPawn)
{
	//ICampInteractionInterface::Interact_Implementation(InstigatorPawn);

	bool bAddedItem;

	if (InstigatorPawn)
	{
		if (const ACampCharacter* CampCharacter = Cast<ACampCharacter>(InstigatorPawn))
		{
			UE_LOG(LogTemp, Display, TEXT("Touched item :O"));

			// If wearing a backpack, default to adding items to that.
			if (const ACampBackpack* Backpack = CampCharacter->GetCampInteractComp()->GetHeldBackpack())
			{
				bAddedItem = Backpack->GetCampInventoryComp()->AddItemToInventory(WorldItemName, 1, bIsStackable);
			}
			// Otherwise, add it to the player's pockets.
			else
			{
				// Add item to inventory map.
				bAddedItem = CampCharacter->GetCampInventoryComp()->AddItemToInventory(WorldItemName, 1, bIsStackable);
			}

			// Destroy item once we've picked it up (if we pick it up).
			if (bAddedItem) Destroy();
		}
	}
}

