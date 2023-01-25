// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCampWorldUtilityItem.h"
#include "CampGameModeBase.h"
#include "CampWorldItem.h"

AMyCampWorldUtilityItem::AMyCampWorldUtilityItem()
{
	ItemType = EItemType::ITEM_Tool;
}

void AMyCampWorldUtilityItem::BeginPlay()
{
	Super::BeginPlay();
}

void AMyCampWorldUtilityItem::Interact_Implementation(APawn* InstigatorPawn)
{
	UE_LOG(LogTemp, Warning, TEXT("CampWorldUtilityItem"));

	SpawnInteractMenu(InstigatorPawn);

	//Super::Interact_Implementation(InstigatorPawn);
}

// BlueprintNativeEvent.
void AMyCampWorldUtilityItem::SpawnInteractMenu_Implementation(APawn* InstigatorPawn)
{
}
