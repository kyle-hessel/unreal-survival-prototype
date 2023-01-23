// Fill out your copyright notice in the Description page of Project Settings.


#include "CampCharacterAnimInstance.h"
#include "CampCharacter.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Math/Vector.h"
//#include "AnimGraphRuntime/Public/KismetAnimationLibrary.h"

void UCampCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner(); // Get the owning pawn of this AnimInstance, which will be our CampCharacter.

		if (Pawn) CampCharacter = Cast<ACampCharacter>(Pawn); // Cast our Pawn to a CampCharacter.

		if (CampCharacter) MovementComp = CampCharacter->GetCharacterMovement();
	}
}

// Configure some animation properties at start of AnimBP, called on BeginPlay in BP.
void UCampCharacterAnimInstance::SetupAnimationProperties()
{
	if (Pawn == nullptr) Pawn = TryGetPawnOwner();

	if (Pawn)
	{
		if (CampCharacter == nullptr) CampCharacter = Cast<ACampCharacter>(Pawn);
		
		if (MovementComp == nullptr) MovementComp = CampCharacter->GetCharacterMovement();
	}

	// Set DefaultMaxMovementSpeed equal to the max speed on CampCharacter's CharacterMovementComponent at game start (same as value set in editor on the BP).
	if (CampCharacter && MovementComp)
	{
		DefaultMaxMovementSpeed = MovementComp->GetMaxSpeed();
		SpeedMultiplier = CampCharacter->SpeedMultiplier;
		ExhaustionMultiplier = CampCharacter->ExhaustionMultiplier;
	}
}

// Update animation properties every frame
void UCampCharacterAnimInstance::UpdateAnimationProperties()
{
	if (Pawn == nullptr) Pawn = TryGetPawnOwner();

	if (Pawn)
	{
		// Fetch our movement velocity, acceleration, vector velocity length, 
		MovementVelocityVector = Pawn->GetVelocity();
		
		if (CampCharacter && MovementComp) MovementAccelerationVector = MovementComp->GetCurrentAcceleration();
		
		MovementSpeedMagnitude = Pawn->GetVelocity().Length();												  // Note: seems like .Length() and .Size() do pretty much the same thing here.
		LateralMovementSpeedMagnitude = FVector(Pawn->GetVelocity().X, Pawn->GetVelocity().Y, 0.f).Size(); // Discard our pawn's vertical velocity and get the magnitude (length) of our lateral vector.

		// Determine if we are falling.
		bIsInAir = Pawn->GetMovementComponent()->IsFalling();
		
		if (CampCharacter && MovementComp)
		{
			// Get movement direction based off of values velocity values we've obtained above and the actor's rotation
			MovementDirection = CalculateDirection(MovementVelocityVector, CampCharacter->GetActorRotation()); //*** Apparently this is deprecated, but below version has an error for now
			//MovementDirection = UKismetAnimationLibrary::CalculateDirection(MovementVelocityVector, CampCharacter->GetActorRotation());

			SpeedMultiplier = CampCharacter->SpeedMultiplier;
			ExhaustionMultiplier = CampCharacter->ExhaustionMultiplier;

			MovementMultiplier = SpeedMultiplier * ExhaustionMultiplier;
			
			// Fetch our current max movement speed from the CampCharacter's CharacterMovementComponent
			MaxMovementSpeed = MovementComp->GetMaxSpeed();

			//UE_LOG(LogTemp, Warning, TEXT("REEEEEE: %f"), MaxMovementSpeed);
			
			// If our character is accelerating set bIsAccelerating to true so that we can use this information in our animation state machine in the character's animation blueprint.
			if (MovementComp->GetCurrentAcceleration().Size() > 0) bIsAccelerating = true; else bIsAccelerating = false;
		}
		
	}
}

void UCampCharacterAnimInstance::DetermineMovementState()
{
	// Normalize our velocity and acceleration vectors before calculating their dot product.
	MovementAccelerationVector.Normalize();
	MovementVelocityVector.Normalize();
	const float MovementAccelerationVectorLength = MovementAccelerationVector.Length();
	
	const float MovementDotProduct = FVector::DotProduct(MovementAccelerationVector, MovementVelocityVector);

	// If the dot product of our two vectors results in a negative ratio that is lower than our IdleThreshold, set LocomotionState to LOCO_Idle from the ELocomotionState Enumerator.
	if (MovementDotProduct < IdleThreshold && !bIsInAir)
	{
		LocomotionState = ELocomotionState::LOCO_Idle;
	}
	// If we are not idle, then check if we jumping. ***(will likely need refactor later to differentiate between falling and jumping then falling)
	else if (bIsInAir)
	{
		LocomotionState = ELocomotionState::LOCO_Jump;
	}
	// If we are not idle or jumping, check to see what form of ground locomotion we are in.
	else
	{
		/* If we are moving
		 * AND our MaxMovementSpeed has gone above what we initially set it to in the editor (250.0 cm/s)
		 * AND our acceleration vector length (how fast we are accelerating) is greater than 0.5
		 * AND we are not in midair set state to LOCO_Run.
		*/
		if (MovementSpeedMagnitude > 1.f && (MaxMovementSpeed > DefaultMaxMovementSpeed * MovementMultiplier) && MovementAccelerationVectorLength > 0.5f && !bIsInAir)
		{
			LocomotionState = ELocomotionState::LOCO_Run;
		}
		// If we are not running, then...
		else
		{
			// If we are moving at all but not quicker than our initial MaxMovementSpeed, set state to LOCO_Walk.
			if (MovementSpeedMagnitude > 1.f && (MaxMovementSpeed > 0.f && (MaxMovementSpeed <= DefaultMaxMovementSpeed * MovementMultiplier)) && MovementAccelerationVectorLength > 0.01f && !bIsInAir)
			{
				LocomotionState = ELocomotionState::LOCO_Walk;
			}
			// Set our state to jumping if we evaluate this far and we are in midair. ***(will likely need refactor later to differentiate between falling and jumping then falling)
			else if (bIsInAir)
			{
				LocomotionState = ELocomotionState::LOCO_Jump;
			}
			// Set our state back to idle if we evaluate this far.
			else
			{
				LocomotionState = ELocomotionState::LOCO_Idle;
			}
		}
	}
}


