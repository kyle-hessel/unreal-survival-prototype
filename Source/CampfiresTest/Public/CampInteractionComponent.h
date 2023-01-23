// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CampInteractionComponent.generated.h"

class ACampBackpack;
class ACampCampsite;
class ACampCharacter;
class APlayerController;
class UPrimitiveComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CAMPFIRESTEST_API UCampInteractionComponent final : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<ACampCharacter> CampCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipables", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<ACampBackpack> HeldBackpack;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactables", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<ACampCampsite> CurrentCampsite;

	UPROPERTY()
	TObjectPtr<AActor> HitActor;

	UPROPERTY()
	TObjectPtr<UPrimitiveComponent> HitComponent;

public:	
	// Sets default values for this component's properties
	UCampInteractionComponent();

	// Getters
	UFUNCTION(BlueprintPure)
	FORCEINLINE ACampBackpack* GetHeldBackpack() const { if (HeldBackpack) return HeldBackpack; return nullptr; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE ACampCampsite* GetCurrentCampsite() const { if (CurrentCampsite) return CurrentCampsite; return nullptr; }

	// Setters
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetCurrentCampsite(ACampCampsite* Campsite) { CurrentCampsite = Campsite; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	//UPROPERTY()
	//APlayerController* PlayerController;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void PrimaryInteract();
	
	void DropEquipable();

	UFUNCTION(BlueprintCallable, Category = "Equipables")
	bool HoldingBackpackOrNot() const;
		
};
