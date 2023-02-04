// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CampItemPlacementInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCampItemPlacementInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface class for defining how different utility items, etc. orient themselves when dropped on the ground.
 */
class CAMPFIRESTEST_API ICampItemPlacementInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, Category = "Interact")
	void OrientItem(FRotator PlayerRotation);
};
