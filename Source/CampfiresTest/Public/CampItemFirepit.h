// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyCampWorldUtilityItem.h"
#include "CampItemFirepit.generated.h"

class UNiagaraComponent;

UCLASS()
class CAMPFIRESTEST_API ACampItemFirepit final : public AMyCampWorldUtilityItem
{
	GENERATED_BODY()

public:
	ACampItemFirepit();

protected:
	// Override of parent function in ACampWorldUtilityItem, itself inheriting up the tree from an interface.
	void Interact_Implementation(APawn* InstigatorPawn) override;

	void OrientItem_Implementation(FRotator PlayerRotation) override;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Interact")
	TObjectPtr<UStaticMeshComponent> KindlingMesh;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Effects")
	TObjectPtr<UNiagaraComponent> CampfireSystem;
};
