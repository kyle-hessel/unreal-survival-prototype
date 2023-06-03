// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CampInteractionInterface.h"
#include "GameFramework/Actor.h"
#include "CampBackpack.generated.h"

class USkeletalMeshComponent;
class UCampInventoryComponent;
class UBoxComponent;
class APawn;
class ACampCharacter;
class UWidgetComponent;
class UUserWidget;

UCLASS()
class CAMPFIRESTEST_API ACampBackpack : public AActor, public ICampInteractionInterface // Keep in mind CampBackpack is both an interactable and equipable object
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<ACampCharacter> CampCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components | Inventory", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UCampInventoryComponent> InventoryComp;

	// Implementation of ICampInteractionInterface's Interact function. '_Implementation' is a result of BlueprintNativeEvent.
	void Interact_Implementation(APawn* InstigatorPawn) override;
	
public:	
	// Sets default values for this actor's properties
	ACampBackpack();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI")
	TObjectPtr<UWidgetComponent> Icon;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> IconClass;

	// Getters
	UFUNCTION(BlueprintPure)
	FORCEINLINE ACampCharacter* GetOwningCharacter() const { return CampCharacter; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE UCampInventoryComponent* GetCampInventoryComp() const { return InventoryComp; }

	// Setters
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetOwningCharacter(ACampCharacter* Character) { CampCharacter = Character; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	bool bIsEquipped;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	float EnergyExpenditureModifier;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float PreviousEnergyDelta;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Positioning")
	TObjectPtr<UBoxComponent> OffsetBox;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	TObjectPtr<USkeletalMeshComponent> BackpackMesh;
			
	FHitResult* HitResult;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintNativeEvent)
	bool DropBackpack();

private:

	void UpdateBackpackLocRot();
	
};
