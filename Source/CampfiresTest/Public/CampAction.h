// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CampAction.generated.h"


/**
 * Generic Action class used for starting and stopping actions in conjunction with ActionComponent.
 */
UCLASS(Blueprintable) // Enable blueprint classes to inherit from this class
class CAMPFIRESTEST_API UCampAction : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, Category = "Actions")
	void StartAction(AActor* Instigator);

	UFUNCTION(BlueprintNativeEvent, Category = "Actions")
	void StopAction(AActor* Instigator);

	UFUNCTION(BlueprintNativeEvent, Category = "Actions")
	void TickAction(AActor* Instigator, float DeltaTime);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Actions")
	FName ActionName;

	UPROPERTY(EditDefaultsOnly, Category = "Actions")
	bool bIsTickable;

	UPROPERTY(VisibleDefaultsOnly, Category = "Actions")
	bool bActionIsTicking;
	
};
