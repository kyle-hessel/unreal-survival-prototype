// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CampInteractionInterface.h"
#include "GameFramework/Actor.h"
#include "CampWorldItem.generated.h"

enum class EItemType : uint8;
class UStaticMeshComponent;
class UWidgetComponent;
class ACampCharacter;
class UUserWidget;

// Generic parent class for any item in the world that the player can interact with that isn't a backpack or campsite (for now).
UCLASS()
class CAMPFIRESTEST_API ACampWorldItem : public AActor, public ICampInteractionInterface  //CampWorldItem is only an interactable.
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACampWorldItem();
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Root")
	TObjectPtr<USceneComponent> DefaultRoot;

	// Generic item mesh, can be anything really.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> Item;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI")
	TObjectPtr<UWidgetComponent> Icon;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> IconClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	int32 ItemID;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	int32 WorldItemIdentifier;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes")
	FName WorldItemName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes")
	FText DisplayName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes")
	FText DisplayDescription;

	// Determines if the item can be stored in a CampInventoryComponent.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attributes")
	bool bIsHoldable;

	// Determines if the item is stackable in inventories.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attributes")
	bool bIsStackable;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attributes")
	EItemType ItemType;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attributes")
	bool bEnergyItem;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attributes")
	float EnergyDelta;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attributes")
	bool bLifeForceItem;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attributes")
	float LifeForceDelta;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attributes")
	bool bCraftable;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Implementation of ICampInteractionInterface's Interact function. '_Implementation' is a result of BlueprintNativeEvent.
	void Interact_Implementation(APawn* InstigatorPawn) override;
	
public:	

	UFUNCTION(BlueprintCallable)
	void AddToInventoryAndDespawn(ACampCharacter* CampCharacter);

};
