// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CampWorldItem.h"
#include "MyCampWorldUtilityItem.generated.h"

class UUserWidget;

/**
 * 
 */
UCLASS()
class CAMPFIRESTEST_API AMyCampWorldUtilityItem : public ACampWorldItem
{
	GENERATED_BODY()

public:
	AMyCampWorldUtilityItem();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Crafting")
	TMap<FName, int32> IngredientsToCraft;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent, Category = "Interact")
	void SpawnInteractMenu(APawn* InstigatorPawn);

	// Override of parent function in ACampWorldItem, which itself is an implementation of ICampInteractionInterface's Interact function.
	void Interact_Implementation(APawn* InstigatorPawn) override;
};
