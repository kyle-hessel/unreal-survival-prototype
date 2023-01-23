// Fill out your copyright notice in the Description page of Project Settings.


#include "CampAction.h"

void UCampAction::StartAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Log, TEXT("Running: %s"), *GetNameSafe(this));

	if (bIsTickable) bActionIsTicking = true;
}

void UCampAction::StopAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Log, TEXT("Stopped: %s"), *GetNameSafe(this));

	if (bIsTickable) bActionIsTicking = false;
}

void UCampAction::TickAction_Implementation(AActor* Instigator, float DeltaTime)
{
	//UE_LOG(LogTemp, Warning, TEXT("Action: %s is ticking!"), *GetNameSafe(this));
}
