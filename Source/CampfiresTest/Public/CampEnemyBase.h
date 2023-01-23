// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CampEnemyBase.generated.h"

class USphereComponent;
class UCapsuleComponent;
class UWidgetComponent;
class UCampEnemyAttributeComponent;
class UAnimMontage;

UCLASS()
class CAMPFIRESTEST_API ACampEnemyBase : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components | Attributes", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UCampEnemyAttributeComponent> AttributeComp;

public:
	// Sets default values for this character's properties
	ACampEnemyBase();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<USphereComponent> AggroSphere;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<USphereComponent> AttackSphere;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UCapsuleComponent> BoundsCapsule;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Combat")
	TObjectPtr<UWidgetComponent> LockOnReticle;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> TakeDamageAnimation;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> TakeBigDamageAnimation;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> DieAnimation;

	// Getters
	UFUNCTION(BlueprintPure)
	FORCEINLINE UCampEnemyAttributeComponent* GetEnemyAttributeComp() const { return AttributeComp; }

	
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Combat")
	bool bLockedOn;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bDead;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void Die();

};
