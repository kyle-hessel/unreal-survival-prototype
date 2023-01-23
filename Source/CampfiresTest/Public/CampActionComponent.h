// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CampActionComponent.generated.h"

class UCampAction;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CAMPFIRESTEST_API UCampActionComponent final : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UCampActionComponent();

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void AddAction(TSubclassOf<UCampAction> ActionClass);

	UFUNCTION(BlueprintCallable, Category = "Actions")
	bool StartActionByName(AActor* Instigator, FName ActionName, bool bIsTickable);

	UFUNCTION(BlueprintCallable, Category = "Actions")
	bool StopActionByName(AActor* Instigator, FName ActionName);

protected:

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UCampAction>> Actions;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<AActor> ActionInstigator;
	
	virtual void BeginPlay() override;

public:
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


private:

	bool bTickingOrNot = false;

	void CheckIfTicking();
	
};
