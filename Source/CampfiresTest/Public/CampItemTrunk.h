// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyCampWorldUtilityItem.h"
#include "CampItemTrunk.generated.h"

class UCampInventoryComponent;
class ACampCharacter;

/**
 * 
 */
UCLASS()
class CAMPFIRESTEST_API ACampItemTrunk final : public AMyCampWorldUtilityItem
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components | Inventory", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UCampInventoryComponent> InventoryComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Instigator", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<ACampCharacter> CampCharacter;

	float CurrentLidRoll;
	float OpenTarget;
	float ClosedTarget;

public:
	ACampItemTrunk();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> LidMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lid")
	float OpenSpeed;

	// Getters
	UFUNCTION(BlueprintPure)
	FORCEINLINE ACampCharacter* GetInstigatorPawn() const { return CampCharacter; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE UCampInventoryComponent* GetCampInventoryComp() const { return InventoryComp; }

protected:
	virtual void BeginPlay() override;
	
	// Override of parent function in ACampWorldUtilityItem, itself inheriting up the tree from an interface.
	void Interact_Implementation(APawn* InstigatorPawn) override;

	void OrientItem_Implementation(FRotator PlayerRotation) override;
	
	virtual void BeginBoxOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	
	virtual void EndBoxOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
};
