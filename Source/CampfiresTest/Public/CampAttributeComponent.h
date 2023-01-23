// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CampAttributeComponent.generated.h"

class ACampCharacter;
class UCampCharacterAnimInstance;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CAMPFIRESTEST_API UCampAttributeComponent final : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCampAttributeComponent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes | Health")
	float CurrentLifeForce;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes | Health")
	float MaxLifeForce;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes | Energy")
	float CurrentEnergy;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes | Energy")
	float MaxEnergy;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes | Modifiers")
	float TemporaryEnergy;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes | Modifiers")
	float EnergyExpenditure;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes | Modifiers")
	float EnergyRecoveryFractionalMultiplier;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes | Modifiers")
	float DefaultEnergyDelta;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes | Modifiers")
	float EnergyRegenCooldown;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Attributes | Modifiers")
	float EnergyRegenTimer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes | Modifiers")
	float LifeForceImpactCooldown;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Attributes | Modifiers")
	float LifeForceImpactTimer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes | Modifiers")
	float DefaultLifeForceDelta;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Time")
	float DeltaTimeScaleMultiplier;


protected:

	UPROPERTY()
	TObjectPtr<ACampCharacter> CampCharacter;

	UPROPERTY()
	TObjectPtr<UCampCharacterAnimInstance> CampCharacterAnimComp;
	
public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool ApplyLifeForceChange(float Delta);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool ApplyEnergyChange(float Delta);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void UpdateCurrentEnergy(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void DetermineExhaustionMultiplier();

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void DetermineLifeForceReduction(float DeltaTime);
		
};
