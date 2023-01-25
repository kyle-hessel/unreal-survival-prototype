// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CampWorldItem.h"
#include "MyCampWorldUtilityItem.generated.h"

/**
 * 
 */
UCLASS()
class CAMPFIRESTEST_API AMyCampWorldUtilityItem : public ACampWorldItem
{
	GENERATED_BODY()

public:
	AMyCampWorldUtilityItem();

protected:
	virtual void BeginPlay() override;

	// Meant to be overwritten in blueprint by child classes, even if they are child BPs of child C++ classes to this class (e.g. CampItemTent -> BP_TentItem)
	UFUNCTION(BlueprintNativeEvent, Category = "Interact")
	void SpawnInteractMenu(APawn* InstigatorPawn);

	// Override of parent function in ACampWorldItem, which itself is an implementation of ICampInteractionInterface's Interact function.
	void Interact_Implementation(APawn* InstigatorPawn) override;
};
