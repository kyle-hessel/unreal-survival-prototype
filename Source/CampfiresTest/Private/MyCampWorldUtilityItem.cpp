// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCampWorldUtilityItem.h"

#include "CampCharacter.h"
#include "CampGameModeBase.h"
#include "CampWorldItem.h"

AMyCampWorldUtilityItem::AMyCampWorldUtilityItem()
{
	ItemType = EItemType::ITEM_Tool;

	Item->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Item->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
}

void AMyCampWorldUtilityItem::BeginPlay()
{
	Super::BeginPlay();
}

void AMyCampWorldUtilityItem::Interact_Implementation(APawn* InstigatorPawn)
{
	UE_LOG(LogTemp, Warning, TEXT("CampWorldUtilityItem"));

	// Spawn an interact menu that lets the player either interact with the item in a meaningful way, OR pick it up.
	if (ACampCharacter* CampCharacter = Cast<ACampCharacter>(InstigatorPawn))
	{
		CampCharacter->ToggleInteractMenu();
		CampCharacter->bInInteractMenu = !CampCharacter->bInInteractMenu;
	}

	//Super::Interact_Implementation(InstigatorPawn); // Calls ACampWorldItem Interact, which adds item to player's inventory. 
}
