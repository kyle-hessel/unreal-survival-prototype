// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CampEnemyAttributeComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CAMPFIRESTEST_API UCampEnemyAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCampEnemyAttributeComponent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes | Health")
	float CurrentLifeForce;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes | Health")
	float MaxLifeForce;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool TakeDamage(float DamageDelta);
		
};
