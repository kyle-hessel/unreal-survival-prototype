// Fill out your copyright notice in the Description page of Project Settings.


#include "CampEnemyAttributeComponent.h"

// Sets default values for this component's properties
UCampEnemyAttributeComponent::UCampEnemyAttributeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	MaxLifeForce = 200.f;
	CurrentLifeForce = MaxLifeForce;
}


// Called when the game starts
void UCampEnemyAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

// This function is only called from CampAnimNotifyStateMeleeAttack if the enemy is not yet marked as dead with bDead, so it should only run if the enemy is alive.
bool UCampEnemyAttributeComponent::TakeDamage(float DamageDelta)
{
	const float LifeForceCutoffThreshold = FMath::Abs(DamageDelta) * 1.5f;
	
	// Only modify our current life force with ApplyLifeForceChange if we are not exceeding the upper or lower bounds (0.0 to MaxLifeForce).
	// Specify a return value of true or false to let the modifying class know if this succeeded or not.
	if (CurrentLifeForce + DamageDelta <= MaxLifeForce && CurrentLifeForce + DamageDelta >= 0.0f)
	{
		CurrentLifeForce += DamageDelta;
		return true;
	}

	// If life force drops below 0, set it back to 0 (might need tweaking).
	if (CurrentLifeForce < LifeForceCutoffThreshold)
	{
		CurrentLifeForce = 0.0f;
		return true;
	}
	
	// If life force goes above MaxLifeForce, set it back to that value (might need tweaking).
	if (CurrentLifeForce > MaxLifeForce - LifeForceCutoffThreshold)
	{
		CurrentLifeForce = MaxLifeForce;
		return true;
	}

	// If none of the above statements returned true, notify us.
	//UE_LOG(LogTemp, Warning, TEXT("Could not modify CurrentLifeForce"));
	return false;
	
}


// Called every frame
void UCampEnemyAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
}

