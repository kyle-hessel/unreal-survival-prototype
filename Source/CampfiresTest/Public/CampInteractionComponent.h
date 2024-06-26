// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CampInteractionComponent.generated.h"

class ACampMeleeWeapon;
class ACampItemBench;
class AMyCampWorldUtilityItem;
class ACampBackpack;
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
	TObjectPtr<AMyCampWorldUtilityItem> CurrentUtilityItem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactables", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<ACampItemBench> CurrentSittingItem;

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
	FORCEINLINE AMyCampWorldUtilityItem* GetCurrentUtilityItem() const { if (CurrentUtilityItem) return CurrentUtilityItem; return nullptr; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE ACampItemBench* GetCurrentSittingItem() const { if (CurrentSittingItem) return CurrentSittingItem; return nullptr; }

	// Setters
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetCurrentUtilityItem(AMyCampWorldUtilityItem* UtilityItem) { CurrentUtilityItem = UtilityItem; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetCurrentSittingItem(ACampItemBench* SittingItem) { CurrentSittingItem = SittingItem; }

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

	UPROPERTY(BlueprintReadOnly, Category = "Interactables")
	FText InteractableName;
		
};
