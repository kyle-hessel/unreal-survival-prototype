// Fill out your copyright notice in the Description page of Project Settings.


#include "CampActionComponent.h"
#include "CampAction.h"


UCampActionComponent::UCampActionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UCampActionComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCampActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Check if any of our actions are ticking every frame to decide if we then call our respective ticking actor's tick functions every frame. This will update bTickingOrNot accordingly.
	CheckIfTicking();

	// If we are indeed ticking, loop through all of our actions and call their respective tick functions each frame.
	if (bTickingOrNot == true)
	{
		for (UCampAction* Action : Actions)
		{
			if (Action && ActionInstigator != nullptr && Action->bIsTickable == true)
			{
				if (Action->bActionIsTicking) Action->TickAction(ActionInstigator, DeltaTime);
			}
		}
	}
}

void UCampActionComponent::AddAction(TSubclassOf<UCampAction> ActionClass)
{
	if (!ensure(ActionClass)) return; // If ActionClass is a nullptr, return

	// Instantiate a new object of type UCampAction and store it in NewAction
	UCampAction* NewAction = NewObject<UCampAction>(this, ActionClass);

	// If NewAction was successfully instantiated, add it to our Actions array.
	if (ensure(NewAction))
	{
		Actions.Add(NewAction);
	}
}

bool UCampActionComponent::StartActionByName(AActor* Instigator, FName ActionName, bool bIsTickable)
{
	// Loop through our array of actions and find the action that matches what we passed into this function, then start that action on the instigator.
	for (UCampAction* Action : Actions)
	{
		if (Action && Action->ActionName == ActionName)
		{
			ActionInstigator = Instigator; // A copy is made here so that our tick function can access our instigator actor without passing it in directly.
			
			Action->StartAction(Instigator);

			if (bIsTickable == Action->bIsTickable) bTickingOrNot = true;
			
			return true; // Return true if we successfully started the action on the instigator.
		}
	}

	return false; // Return false if our for loop never found the given action in the actions array.
}

bool UCampActionComponent::StopActionByName(AActor* Instigator, FName ActionName)
{
	
	// Loop through our array of actions and find the action that matches what we passed into this function, then stop that action.
	for (UCampAction* Action : Actions)
	{
		if (Action && Action->ActionName == ActionName)
		{
			Action->StopAction(Instigator);
			
			return true; // Return true if we successfully stopped the action.
		}
	}

	return false; // Return false if our for loop never found the given action in the actions array.
}

/* Check every frame if any of the actions in our actions array are actively ticking. This function sets bTickingOrNot
 * so that we don't call TickAction functions on our respective CampActions more than necessary.
*/
void UCampActionComponent::CheckIfTicking()
{
	int8 TickingCount = 0;
	
	if (Actions.Num() > 0)
	{
		for (const UCampAction* Action : Actions)
		{
			if (Action) if (Action->bActionIsTicking) ++TickingCount;

			if (TickingCount > 0) bTickingOrNot = true; else bTickingOrNot = false;
		}
	}
}

