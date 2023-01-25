// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameFramework/GameModeBase.h"
#include "CampGameModeBase.generated.h"

class UTexture2D;
class UStaticMesh;
class UDataTable;

// Create an enumerator for different item types. Determines how energy and health are applied, if at all.
UENUM(BlueprintType)
enum class EItemType : uint8
{
	ITEM_FoodRaw		UMETA(DisplayName="Raw Food"),
	ITEM_Food			UMETA(DisplayName="Food"),
	ITEM_Drink			UMETA(DisplayName="Drink"),
	ITEM_Tool			UMETA(DisplayName="Tool"),
	ITEM_Resource		UMETA(DisplayName="Resource"),
};

// Struct for inventory data table
USTRUCT(BlueprintType)
struct FItemStruct : public FTableRowBase // Derive from FTableRowBase to make this struct a datatable.
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item | Attributes")
	FText DisplayName;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item | Attributes")
	FText DisplayDescription;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item | Attributes")
	TObjectPtr<UTexture2D> Thumbnail;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item | Attributes")
	UStaticMesh* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item | Attributes")
	bool bIsStackable;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item | Attributes")
	EItemType ItemType;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item | Attributes")
	bool bEnergyItem;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item | Attributes")
	bool bLifeForceItem;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item | Attributes")
	float EnergyDelta;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item | Attributes")
	float LifeForceDelta;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item | Attributes")
	bool bCraftable;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item | Crafting")
	TMap<FName, int32> IngredientsToCraft;

	// Struct defaults for members.
	FItemStruct()
	{
		DisplayName = FText::FromString("Item");
		DisplayDescription = FText::FromString("Description");
		Thumbnail = nullptr;
		Mesh = nullptr;
		bIsStackable = false;
		ItemType = EItemType::ITEM_Resource;
		bEnergyItem = false;
		bLifeForceItem = false;
		EnergyDelta = 0.0f;
		LifeForceDelta = 0.0f;
		bCraftable = false;
	}
	
};

inline UDataTable* Items;

/**
 * 
 */
UCLASS()
class CAMPFIRESTEST_API ACampGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACampGameModeBase();

	UFUNCTION()
	void PopulateItemsDataTable() const;
	
};
