// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CampWorldItem.h"
#include "MyCampWorldUtilityItem.generated.h"

class UBoxComponent;

/**
 * 
 */
UCLASS()
class CAMPFIRESTEST_API AMyCampWorldUtilityItem : public ACampWorldItem
{
	GENERATED_BODY()

public:
	AMyCampWorldUtilityItem();

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Access")
	TObjectPtr<UBoxComponent> AccessBox;

protected:
	virtual void BeginPlay() override;

	// Override of parent function in ACampWorldItem, which itself is an implementation of ICampInteractionInterface's Interact function.
	void Interact_Implementation(APawn* InstigatorPawn) override;

	UFUNCTION()
	virtual void BeginBoxOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	virtual void EndBoxOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
};
