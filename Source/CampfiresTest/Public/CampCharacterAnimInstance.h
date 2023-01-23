// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CampCharacterAnimInstance.generated.h"

class APawn;
class ACampCharacter;
class UCharacterMovementComponent;

// Create an enumerator for different movement states - groundwork for our animation state machine.
UENUM(BlueprintType)
enum class ELocomotionState : uint8
{
	LOCO_Idle		UMETA(DisplayName="Idle"),
	LOCO_Walk		UMETA(DisplayName="Walk"),
	LOCO_Run		UMETA(DisplayName="Run"),
	LOCO_Jump		UMETA(DisplayName="Jump"),
};

/**
 * Configure animation functionality for the animation blueprint that drives the traveler.
 */
UCLASS()
class CAMPFIRESTEST_API UCampCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<APawn> Pawn;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<ACampCharacter> CampCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UCharacterMovementComponent> MovementComp;

public:

	virtual void NativeInitializeAnimation() override;

	UFUNCTION(BlueprintCallable, Category = "Animation | Properties")
	void SetupAnimationProperties();
	
	UFUNCTION(BlueprintCallable, Category = "Animation | Properties")
	void UpdateAnimationProperties();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void DetermineMovementState();
	
	// Declare our enumerator that we defined above our class.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	ELocomotionState LocomotionState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float LateralMovementSpeedMagnitude;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float MovementSpeedMagnitude;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	FVector MovementVelocityVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	FVector MovementAccelerationVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float MaxMovementSpeed;

	UPROPERTY()
	float DefaultMaxMovementSpeed;

	UPROPERTY()
	float SpeedMultiplier;

	UPROPERTY()
	float ExhaustionMultiplier;

	UPROPERTY()
	float MovementMultiplier;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float MovementDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float IdleThreshold = 0.2f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsInAir;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsAccelerating;
};
