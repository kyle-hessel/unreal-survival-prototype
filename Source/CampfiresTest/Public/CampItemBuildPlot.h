// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CampItemBuildPlot.generated.h"

class ACampCharacter;
class UBoxComponent;
class ACampWorldItem;

UCLASS()
class CAMPFIRESTEST_API ACampItemBuildPlot : public AActor
{
	GENERATED_BODY()

	TObjectPtr<ACampCharacter> CampCharacter;
	
public:	
	ACampItemBuildPlot();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> BoundingBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TMap<FName, int32> ItemsRequired;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void CalculateAcquiredMaterials();

	UFUNCTION()
	void SpawnBuildAndDeleteSelf();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 BuildItemIdentifier;

	UPROPERTY()
	bool bBuildingItem;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<TObjectPtr<ACampWorldItem>> OverlappingItems;

public:

	UFUNCTION()
	void BeginBoxOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void EndBoxOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
