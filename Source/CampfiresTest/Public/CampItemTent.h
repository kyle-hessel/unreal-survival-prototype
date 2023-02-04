// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyCampWorldUtilityItem.h"
#include "CampItemTent.generated.h"

/**
 * 
 */
UCLASS()
class CAMPFIRESTEST_API ACampItemTent final : public AMyCampWorldUtilityItem
{
	GENERATED_BODY()

public:
	ACampItemTent();

protected:
	// Override of parent function in ACampWorldUtilityItem, itself inheriting up the tree from an interface.
	void Interact_Implementation(APawn* InstigatorPawn) override;

	void OrientItem_Implementation(FRotator PlayerRotation) override;
};
