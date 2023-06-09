// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ACampEquipable.generated.h"

class UWidgetComponent;
class UUserWidget;

UCLASS()
class CAMPFIRESTEST_API AACampEquipable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AACampEquipable();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI")
	TObjectPtr<UWidgetComponent> Icon;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> IconClass;

	UPROPERTY(BlueprintReadOnly)
	bool bEquipped;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
