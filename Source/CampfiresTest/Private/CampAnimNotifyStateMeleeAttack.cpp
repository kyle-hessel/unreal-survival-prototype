// Fill out your copyright notice in the Description page of Project Settings.


#include "CampAnimNotifyStateMeleeAttack.h"
#include "CampAttributeComponent.h"
#include "CampCharacter.h"
#include "CampEnemyBase.h"
#include "CampEnemyAttributeComponent.h"
#include "CampMeleeWeapon.h"

void UCampAnimNotifyStateMeleeAttack::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                  float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	CampCharacter = Cast<ACampCharacter>(MeshComp->GetOwner());

	bApplyDamageOnce = true;

	// Mark that an attack animation is playing every time this notify state is triggered.
	if (CampCharacter)
	{
		CampCharacter->bAttackPlaying = true;
		CampCharacterMeleeWeapon = CampCharacter->GetActiveMeleeWeapon(); // We only get to this anim notify if we already have a valid weapon, so we don't need to check validity for this below.
	}
}

void UCampAnimNotifyStateMeleeAttack::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	// Break out this tick early if damage was already applied.
	if (bApplyDamageOnce == false) return;

	// *** Idk if I really need this many checks but I'm playing it safe for now
	if (CampCharacter)
	{
		if (ACampEnemyBase* ReceivingEnemy = CampCharacterMeleeWeapon->GetReceivingEnemy(); ReceivingEnemy && CampCharacterMeleeWeapon->bCollidingWithEnemy)
		{
			// Apply damage, play anim, etc. one time during the duration of this animation once the player's melee weapon collides with the enemy.
			if (bApplyDamageOnce == true && ReceivingEnemy->bDead == false)
			{
				UE_LOG(LogTemp, Warning, TEXT("Hit registered."));

				// Play camera shake.
				CampCharacter->PlayHitCameraShake();
					
				// Determine how much damage to apply and what animation to play depending on attack combo state.
				if (CampCharacter->MeleeComboNumber < 2)
				{
					const float DamageAmount = CampCharacterMeleeWeapon->DefaultDamageDelta;

					// Also determine if we just play a death animation instead if the enemy's health is going to drop to 0.
					if (ReceivingEnemy->GetEnemyAttributeComp()->CurrentLifeForce - DamageAmount <= 0.0f)
					{
						// Play death animation, set enemy's death var to true, mark for deletion, etc.
						ReceivingEnemy->Die();
						CampCharacter->PlayHitFreezeFrame(0.8f, 0.025f); // Play a freeze frame when killing an enemy.

						// If we are locked onto this enemy when they die, release lock-on.
						if (ACampEnemyBase* LockedOnEnemy = CampCharacter->GetLockedOnEnemy(); LockedOnEnemy == ReceivingEnemy)
						{
							CampCharacter->ReleaseLockOn(LockedOnEnemy);
						}

						// Remove the enemy from nearby enemies list as well.
						CampCharacter->RemoveNearbyEnemy(ReceivingEnemy);
					}
					else
					{
						ReceivingEnemy->PlayAnimMontage(ReceivingEnemy->TakeDamageAnimation);
					}
						
					ReceivingEnemy->GetEnemyAttributeComp()->TakeDamage(-DamageAmount);
					
					UE_LOG(LogTemp, Warning, TEXT("New enemy health: %f"), ReceivingEnemy->GetEnemyAttributeComp()->CurrentLifeForce);
				}
				else if (CampCharacter->MeleeComboNumber == 2)
				{
					const float DamageAmount = CampCharacterMeleeWeapon->DefaultDamageDelta * CampCharacterMeleeWeapon->ComboTwoDmgModifier;

					if (ReceivingEnemy->GetEnemyAttributeComp()->CurrentLifeForce - DamageAmount <= 0.0f)
					{
						ReceivingEnemy->Die();
						CampCharacter->PlayHitFreezeFrame(0.8f, 0.025f);
							
						if (ACampEnemyBase* LockedOnEnemy = CampCharacter->GetLockedOnEnemy(); LockedOnEnemy == ReceivingEnemy)
						{
							CampCharacter->ReleaseLockOn(LockedOnEnemy);
						}
							
						CampCharacter->RemoveNearbyEnemy(ReceivingEnemy);
					}
					else
					{
						ReceivingEnemy->PlayAnimMontage(ReceivingEnemy->TakeDamageAnimation);
					}

					ReceivingEnemy->GetEnemyAttributeComp()->TakeDamage(-DamageAmount);

					UE_LOG(LogTemp, Warning, TEXT("New enemy health: %f"), ReceivingEnemy->GetEnemyAttributeComp()->CurrentLifeForce);
				}
				else if (CampCharacter->MeleeComboNumber == 3)
				{
					const float DamageAmount = CampCharacterMeleeWeapon->DefaultDamageDelta * CampCharacterMeleeWeapon->ComboThreeDmgModifier;

					if (ReceivingEnemy->GetEnemyAttributeComp()->CurrentLifeForce - DamageAmount <= 0.0f)
					{
						ReceivingEnemy->Die();
						CampCharacter->PlayHitFreezeFrame(0.8f, 0.025f);
						
						if (ReceivingEnemy)
						{
							CampCharacter->ReleaseLockOn(CampCharacter->GetLockedOnEnemy());
						}
							
						CampCharacter->RemoveNearbyEnemy(ReceivingEnemy);
					}
					else
					{
						ReceivingEnemy->PlayAnimMontage(ReceivingEnemy->TakeBigDamageAnimation);
					}
						
					ReceivingEnemy->GetEnemyAttributeComp()->TakeDamage(-DamageAmount);

					UE_LOG(LogTemp, Warning, TEXT("New enemy health: %f"), ReceivingEnemy->GetEnemyAttributeComp()->CurrentLifeForce);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Invalid melee combo number, wot?"));
				}

				// Ensure that damage isn't applied again every tick, just once the instant collision was detected.
				bApplyDamageOnce = false;
				}
			}
	}
	
}

void UCampAnimNotifyStateMeleeAttack::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	CampCharacter->bAttackPlaying = false;

	// Remove a set amount of energy at the end of every attack in the chain. I'm leaving this as a flat value for now that is set per-weapon and nothing more.
	CampCharacter->GetCampAttributeComp()->ApplyEnergyChange(-CampCharacterMeleeWeapon->DefaultEnergyDelta);

	// If the player didn't click before the given attack animation was up, end the combo.
	if (CampCharacter->bQueueNextAttackInCombo == false)
	{
		CampCharacter->StopAnimMontage(CampCharacter->AttackChainAnimation);
		CampCharacter->MeleeComboNumber = 0;
	}
	// Otherwise, continue the combo.
	else
	{
		// Combo count incrementing for damage application.
		if (CampCharacter->MeleeComboNumber < CampCharacter->MaxComboNumber)
		{
			CampCharacter->MeleeComboNumber++;
		}
		else if (CampCharacter->MeleeComboNumber == CampCharacter->MaxComboNumber)
		{
			CampCharacter->MeleeComboNumber = 0;
		}
			
		UE_LOG(LogTemp, Warning, TEXT("Combo number: %d"), CampCharacter->MeleeComboNumber);
		CampCharacter->bQueueNextAttackInCombo = false;
	}

	//bApplyDamageOnce = true;
	
	Super::NotifyEnd(MeshComp, Animation, EventReference);
}



