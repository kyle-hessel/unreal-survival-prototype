// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CampInteractionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCampInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface class for making any object player-interactable (backpack, item pickups, doors, etc)
 * each interactable class determines how to handle this functionality but inherits this interface.
 */
class CAMPFIRESTEST_API ICampInteractionInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, Category = "Interact")
	void Interact(APawn* InstigatorPawn);
};
