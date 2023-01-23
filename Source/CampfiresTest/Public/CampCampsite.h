// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CampInteractionInterface.h"
#include "GameFramework/Actor.h"
#include "CampCampsite.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UCampInventoryComponent;
class UParticleSystem;
class ACampCharacter;
class UPrimitiveComponent;
class AActor;

UCLASS()
class CAMPFIRESTEST_API ACampCampsite : public AActor, public ICampInteractionInterface  // CampCampsite is only an interactable.
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Instigator", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<ACampCharacter> CampCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components | Inventory", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UCampInventoryComponent> InventoryComp;

	// Implementation of ICampInteractionInterface's Interact function. '_Implementation' is a result of BlueprintNativeEvent.
	void Interact_Implementation(APawn* InstigatorPawn) override;
	
public:	
	ACampCampsite();

	UPROPERTY(VisibleAnywhere, Category = "Boundary")
	TObjectPtr<UBoxComponent> BoundingBox;

	UPROPERTY(VisibleAnywhere, Category = "Campsite")
	TObjectPtr<UStaticMeshComponent> CampfirePit;

	UPROPERTY(VisibleAnywhere, Category = "Campsite")
	TObjectPtr<UParticleSystem> Campfire;

	UPROPERTY(VisibleAnywhere, Category = "Campsite")
	TObjectPtr<UStaticMeshComponent> Tent;

	UPROPERTY(VisibleAnywhere, Category = "Campsite")
	TObjectPtr<UStaticMeshComponent> Bench;

	UPROPERTY(VisibleAnywhere, Category = "Campsite")
	TObjectPtr<UStaticMeshComponent> Trunk;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	TObjectPtr<UBoxComponent> TrunkAccessBox;

	// Getters
	UFUNCTION(BlueprintPure)
	FORCEINLINE ACampCharacter* GetOwningCharacter() const { return CampCharacter; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE UCampInventoryComponent* GetCampInventoryComp() const { return InventoryComp; }

	// Setters
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetOwningCharacter(ACampCharacter* Character) { CampCharacter = Character; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	bool bWithinTrunkRange;

protected:

	virtual void BeginPlay() override;

public:	

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void BeginTrunkOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void EndTrunkOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
