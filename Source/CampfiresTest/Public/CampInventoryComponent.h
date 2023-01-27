// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CampInventoryComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CAMPFIRESTEST_API UCampInventoryComponent final : public UActorComponent
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemPickupDelegate);

	// Key value pairing of inventory item and how many of that item exists (name, count)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", meta=(AllowPrivateAccess = "true"))
	TMap<FName, int32> InventoryContents;

public:	
	// Sets default values for this component's properties
	UCampInventoryComponent();

	UFUNCTION(BlueprintCallable)
	bool AddItemToInventory(FName ItemName, int32 Quantity, bool bStackable);

	UFUNCTION(BlueprintCallable)
	bool RemoveItemFromInventory(FName ItemNameAndNumber, FName ItemName, int32 Quantity);

	UFUNCTION(BlueprintCallable)
	bool DragItemOutOfInventory(FName ItemNameAndNumber, FName ItemName, int32 Quantity);

	UFUNCTION(BlueprintCallable)
	bool SpawnItem(FName ItemName);

	UFUNCTION(BlueprintCallable)
	void ConsumeItem(FName ItemNameAndNumber, const FName ItemName, int32 Quantity) const;

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnItemPickupDelegate OnItemPickup;

	// Getters
	UFUNCTION(BlueprintPure)
	FORCEINLINE void GetInventoryContentsKeys(TArray<FName>& OutKeys) const { InventoryContents.GetKeys(OutKeys); }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Inventory data table
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	//UDataTable* Items;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	FString InventoryName;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Inventory")
	bool bIsOpen;

	UPROPERTY(BlueprintReadWrite, Category = "Inventory")
	TArray<FName> ItemsToConsume;
	
	// Maximum size of the inventory.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	int32 MaxSize;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 ItemIncrement;
};
