// Fill out your copyright notice in the Description page of Project Settings.


#include "CampWorldItem.h"

#include "CampBackpack.h"
#include "CampCharacter.h"
#include "CampInteractionComponent.h"
#include "CampInventoryComponent.h"
#include "CampGameModeBase.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h" 

ACampWorldItem::ACampWorldItem()
{
	PrimaryActorTick.bCanEverTick = false;

	DefaultRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRoot"));
	SetRootComponent(DefaultRoot);

	// Item mesh defaults
	Item = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Item"));
	Item->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Item->SetCollisionObjectType(ECC_GameTraceChannel4);
	Item->SetCollisionResponseToAllChannels(ECR_Overlap);
	Item->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECR_Ignore);
	Item->SetupAttachment(GetRootComponent());

	// Item icon defaults
	Icon = CreateDefaultSubobject<UWidgetComponent>(TEXT("Icon"));
	const FStringClassReference ItemTargetClassRef(TEXT("/Game/CampfiresTest/Blueprints/UI/Interact/WBP_ItemTarget.WBP_ItemTarget_C"));
	Icon->SetWidgetClass(ItemTargetClassRef.TryLoadClass<UUserWidget>());
	Icon->SetWidgetSpace(EWidgetSpace::Screen);
	Icon->SetVisibility(false);
	Icon->SetHiddenInGame(false);
	Icon->SetRelativeLocation(FVector(0.0f, 0.0f, 150.0f));
	Icon->SetupAttachment(GetRootComponent());

	// Set world item defaults, change these per-item in blueprint as necessary.
	bIsHoldable = true;

	ItemID = 0;
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
	const FItemStruct* NewItemData = Items->FindRow<FItemStruct>(RowNames[WorldItemIdentifier], "Create", true);
	WorldItemName = RowNames[WorldItemIdentifier]; // For actual item identification when adding to inventory.
	ItemID = NewItemData->ItemID;
	DisplayName = NewItemData->DisplayName; // For UI
	DisplayDescription = NewItemData->DisplayDescription; // For UI
	bIsStackable = NewItemData->bIsStackable;
	Item->SetStaticMesh(NewItemData->Mesh);
	// Item->SetMaterial() // This or SetMaterialByName will likely be necessary later.
	ItemType = NewItemData->ItemType;
	bEnergyItem = NewItemData->bEnergyItem;
	EnergyDelta = NewItemData->EnergyDelta;
	bLifeForceItem = NewItemData->bLifeForceItem;
	LifeForceDelta = NewItemData->LifeForceDelta;
}

// Triggered when the player interacts with the item through CampInteractionComponent.
void ACampWorldItem::Interact_Implementation(APawn* InstigatorPawn)
{
	//ICampInteractionInterface::Interact_Implementation(InstigatorPawn);

	if (InstigatorPawn)
	{
		if (ACampCharacter* CampCharacter = Cast<ACampCharacter>(InstigatorPawn))
		{
			AddToInventoryAndDespawn(CampCharacter);
		}
	}
}

void ACampWorldItem::AddToInventoryAndDespawn(ACampCharacter* CampCharacter)
{
	UE_LOG(LogTemp, Display, TEXT("Touched item :O"));

	bool bAddedItem;
	
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


