// Fill out your copyright notice in the Description page of Project Settings.


#include "CampAttributeComponent.h"
#include "CampCharacter.h"
#include "CampCharacterAnimInstance.h"

// Sets default values for this component's properties
UCampAttributeComponent::UCampAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	MaxLifeForce = 500.f;
	CurrentLifeForce = MaxLifeForce;

	MaxEnergy = 600.f;
	CurrentEnergy = MaxEnergy;

	/* Energy modifiers. EnergyRegenCooldown is the max length EnergyRegenTimer has to decrement from until energy regen begins from standing still.
	 * So, higher values equals a larger cooldown. DefaultEnergyDelta is somewhat self-explanatory, and TemporaryEnergy is stored at the end of any
	 * movement state so that once the player stands still their energy can only increase to a value relative to what they ended at.
	 * EnergyExpenditure is incremented when moving, and when idling the player can only recover a fraction of this expenditure (EnergyExpenditure * EnergyRecoveryMultiplier)
	 * As long as EnergyRecoveryMultiplier is a fractional value and not greater than 1.0, CurrentEnergy is a finite resource that will eventually run out through movement alone.
	*/ 
	DefaultEnergyDelta = 0.0125f;
	EnergyRegenCooldown = 250.f; // How long it takes limited energy regeneration to begin.
	EnergyRegenTimer = 0.f; // How close we are to regenerating energy while idle; takes as long as EnergyRegenCooldown.
	TemporaryEnergy = CurrentEnergy; // For storing our CurrentEnergy whenever we stop moving: for limited energy regeneration.
	EnergyExpenditure = 0.0f; // Just a default. Accrued while moving to calculate how much energy we consumed.
	EnergyRecoveryFractionalMultiplier = 0.2f; // This value matters a lot: see above. Limits how much energy we get back.

	// Timer for how long it takes to impact health, based on our energy system.
	LifeForceImpactCooldown = 400.f;
	LifeForceImpactTimer = 0.f;
	DefaultLifeForceDelta = 0.04f;

	// For making values increment/decrement faster while still making use of DeltaTime
	DeltaTimeScaleMultiplier = 50.f;
}

void UCampAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Update our current energy each frame, whether that's degeneration or regeneration.
	UpdateCurrentEnergy(DeltaTime);

	// Determine our Exhaustion Multiplier by calculating the ratio of CurrentEnergy to MaxEnergy.
	DetermineExhaustionMultiplier();

	// Determine if life force should be reduced based on our current energy value and a cooldown buffer timer.
	DetermineLifeForceReduction(DeltaTime);
	
	//if (CampCharacter) UE_LOG(LogTemp, Warning, TEXT("exhaustion multiplier: %f"), CampCharacter->ExhaustionMultiplier);
	//UE_LOG(LogTemp, Warning, TEXT("energy: %f"), CurrentEnergy);
}

// Apply a positive or negative change in life force.
bool UCampAttributeComponent::ApplyLifeForceChange(float Delta)
{
	const float LifeForceCutoffThreshold = FMath::Abs(Delta) * 1.5f;
	
	// Only modify our current life force with ApplyLifeForceChange if we are not exceeding the upper or lower bounds (0.0 to MaxLifeForce).
	// Specify a return value of true or false to let the modifying class know if this succeeded or not.
	if (CurrentLifeForce + Delta <= MaxLifeForce && CurrentLifeForce + Delta >= 0.0f)
	{
		CurrentLifeForce += Delta;
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

// Apply a positive or negative change in energy. Called in this class by UpdateCurrentEnergy and outside the class by other energy modifying actions such as eating and combat.
bool UCampAttributeComponent::ApplyEnergyChange(float Delta)
{
	const float EnergyCutoffThreshold = FMath::Abs(Delta) * 1.5f;
	
	if (CurrentEnergy + Delta <= MaxEnergy && CurrentEnergy + Delta >= 0.0f)
	{
		CurrentEnergy += Delta;
		return true;
	}

	// If we drop really closet to 0.f, just set to 0 (might need tweaking).
	if (CurrentEnergy < EnergyCutoffThreshold)
	{
		CurrentEnergy = 0.0f;
		return true;
	}

	// If we get really close to MaxEnergy, just to to MaxEnergy (might need tweaking).
	if (CurrentEnergy > MaxEnergy - EnergyCutoffThreshold)
	{
		CurrentEnergy = MaxEnergy;
		return true;
	}

	// If none of the above statements returned true, notify us.
	//UE_LOG(LogTemp, Warning, TEXT("Could not modify CurrentEnergy"));
	return false;
}

// UpdateCurrentEnergy, called every frame from tick.
void UCampAttributeComponent::UpdateCurrentEnergy(float DeltaTime)
{
	if (CampCharacter == nullptr) CampCharacter = Cast<ACampCharacter>(GetOwner());
	if (CampCharacterAnimComp == nullptr)
	{
		if (CampCharacter) CampCharacterAnimComp = Cast<UCampCharacterAnimInstance>(CampCharacter->GetMesh()->GetAnimInstance());
	}
	
	if (CampCharacterAnimComp)
	{
		// Only apply these changes if we have life force
		if (CurrentLifeForce > 0.0f)
		{
			// Account for DeltaTime
			const float EnergyDelta = DefaultEnergyDelta * DeltaTime * DeltaTimeScaleMultiplier;
			
			// If CampCharacter is walking, decrement energy each frame by a small amount.
			if (CampCharacterAnimComp->LocomotionState == ELocomotionState::LOCO_Walk)
			{
				if (CurrentEnergy > 0.0f)
				{
					ApplyEnergyChange(-EnergyDelta);
					EnergyExpenditure += EnergyDelta;
				}

				// Keep EnergyRegenTimer at max value until we stop this action.
				EnergyRegenTimer = EnergyRegenCooldown;

				// Store the energy we ended at while in this state.
				TemporaryEnergy = CurrentEnergy;
			}
			// If the CampCharacter is running decrement energy faster.
			else if (CampCharacterAnimComp->LocomotionState == ELocomotionState::LOCO_Run)
			{
				if (CurrentEnergy > 0.0f)
				{
					constexpr float EnergySubtractionRunMultiplier = 1.5f;
					
					ApplyEnergyChange(-EnergyDelta * EnergySubtractionRunMultiplier);
					EnergyExpenditure += EnergyDelta * EnergySubtractionRunMultiplier;
				}
			
				EnergyRegenTimer = EnergyRegenCooldown;

				TemporaryEnergy = CurrentEnergy;
			}
			// Also decrement energy if the character is jumping, at a rate between walking and running.
			// This does mean that longer jumps are more expensive.
			else if (CampCharacterAnimComp->LocomotionState == ELocomotionState::LOCO_Jump)
			{
				if (CurrentEnergy > 0.0f)
				{
					constexpr float EnergySubtractionJumpMultiplier = 1.25f;
					
					ApplyEnergyChange(-EnergyDelta * EnergySubtractionJumpMultiplier);
					EnergyExpenditure += EnergyDelta * EnergySubtractionJumpMultiplier;
				}
			
				EnergyRegenTimer = EnergyRegenCooldown;

				TemporaryEnergy = CurrentEnergy;
			}
			/* If we are idle, decrement EnergyRegenTimer until it hits 0. Once it does, begin incrementing energy, but only by a limited amount.
			 * This means if the player takes a lot of breaks they can burn through energy much slower, but it eventually runs out regardless.
			*/
			else if (CampCharacterAnimComp->LocomotionState == ELocomotionState::LOCO_Idle)
			{
				// Account for DeltaTime when decrementing the timer.
				if (EnergyRegenTimer > 0.0f) EnergyRegenTimer -= 1 * DeltaTime * DeltaTimeScaleMultiplier;

				if (EnergyRegenTimer < 0.0f) EnergyRegenTimer = 0.0f;

				//UE_LOG(LogTemp, Display, TEXT("EnergyRegenTimer: %f"), EnergyRegenTimer);

				// if our EnergyRegenTimer has hit 0 then..
				if (EnergyRegenTimer == 0.0f && CurrentEnergy <= MaxEnergy && CurrentEnergy >= 0.0f)
				{
					// If our current energy is less than our temporary energy stored in one of the movement states, only increment up to that value.
					if (CurrentEnergy < TemporaryEnergy + EnergyExpenditure * EnergyRecoveryFractionalMultiplier)
					{
						// Regain energy at a faster rate than how fast we lose it so player doesn't have to take ridiculously long breaks (if they choose to).
						ApplyEnergyChange(EnergyDelta * 2.5f);
					}
					// Only clear EnergyExpenditure from movement once we have regenerated a portion of that energy.
					else
					{
						// Also check if the EnergyRegenTimer ran out or not, so that we don't lose possible regenerated energy if we stop moving and then start again before the timer runs out.
						if (EnergyRegenTimer == 0.0f) EnergyExpenditure = 0.0f;

						// If we reach a really low value, drop CurrentEnergy to 0.0.
						if (CurrentEnergy < 0.1) CurrentEnergy = 0.0f;
					}
				}
			}
		}

		// If we run out of life force, default energy to 0.
		else
		{
			if (CurrentEnergy != 0.0f) CurrentEnergy = 0.0f;
		}
	}
}

// Break the exhaustion multiplier into four tiers relative to CurrentEnergy divided by MaxEnergy.
void UCampAttributeComponent::DetermineExhaustionMultiplier()
{
	if (CampCharacter)
	{
		const float EnergyRatio = CurrentEnergy / MaxEnergy;
		
		if (EnergyRatio > 0.75f)
		{
			CampCharacter->ExhaustionMultiplier = 1.0f;
		}
		else if (EnergyRatio > 0.5f && EnergyRatio <= 0.75f)
		{
			CampCharacter->ExhaustionMultiplier = 0.8f;
		}
		else if (EnergyRatio > 0.25f && EnergyRatio <= 0.5f)
		{
			CampCharacter->ExhaustionMultiplier = 0.6f;
		}
		else if (EnergyRatio >= 0.0f && EnergyRatio <= 0.25f)
		{
			CampCharacter->ExhaustionMultiplier = 0.4f;
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("Could not set EnergyRatio."));
		}
	}
}

// Begin lowering the player's life force if their energy runs out and does not go back up for a certain amount of time.
void UCampAttributeComponent::DetermineLifeForceReduction(float DeltaTime)
{
	if (CampCharacter)
	{
		// If energy is 0, begin cooldown timer to impact life force.
		if (CurrentEnergy == 0.0f)
		{
			// Account for DeltaTime
			const float LifeForceDelta = DefaultLifeForceDelta * DeltaTime * DeltaTimeScaleMultiplier;
			
			if (LifeForceImpactTimer <= 0.f) LifeForceImpactTimer = 0.f;
			else LifeForceImpactTimer -= 1 * DeltaTime * DeltaTimeScaleMultiplier; // Account for DeltaTime when decrementing timer.

			// Once the cooldown timer has run out, begin decrementing health.
			if (LifeForceImpactTimer == 0.f)
			{
				ApplyLifeForceChange(-LifeForceDelta);
			}
		}
		// Reset life force impact timer if energy is above 0.
		else
		{
			LifeForceImpactTimer = LifeForceImpactCooldown;
		}

		//UE_LOG(LogTemp, Display, TEXT("LifeForceImpactTimer: %f"), LifeForceImpactTimer);
		//UE_LOG(LogTemp, Warning, TEXT("Current life force: %f"), CurrentLifeForce);
	}
}
