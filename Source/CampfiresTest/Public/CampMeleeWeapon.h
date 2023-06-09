// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ACampEquipable.h"
#include "CampInteractionInterface.h"
#include "GameFramework/Actor.h"
#include "CampMeleeWeapon.generated.h"

class UStaticMeshComponent;
class USkeletalMeshSocket;
class UBoxComponent;
class USphereComponent;
class ACampCharacter;
class ACampEnemyBase;

UCLASS()
class CAMPFIRESTEST_API ACampMeleeWeapon : public AACampEquipable, public ICampInteractionInterface
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mesh", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> WeaponMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> Hitbox;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<ACampEnemyBase> ReceivingEnemy;

	// Implementation of ICampInteractionInterface's Interact function. '_Implementation' is a result of BlueprintNativeEvent.
	void Interact_Implementation(APawn* InstigatorPawn) override;
	
public:	
	// Sets default values for this actor's properties
	ACampMeleeWeapon();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interact")
	TObjectPtr<USphereComponent> InteractSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Transform")
	TObjectPtr<const USkeletalMeshSocket> HolsterSocket;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Transform")
	TObjectPtr<const USkeletalMeshSocket> CombatSocket;

	// Getters
	UFUNCTION(BlueprintPure)
	FORCEINLINE ACampEnemyBase* GetReceivingEnemy() const { if (ReceivingEnemy) return ReceivingEnemy; return nullptr; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE UBoxComponent* GetHitbox() const { return Hitbox; }
	
	UPROPERTY(BlueprintReadOnly)
	bool bInCombat;

	UPROPERTY(BlueprintReadOnly)
	bool bCollidingWithEnemy;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float DefaultDamageDelta;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float DefaultEnergyDelta;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float ComboTwoDmgModifier;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float ComboThreeDmgModifier;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void BeginHitboxOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void EndHitboxOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void BeginInteractSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void EndInteractSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	

};
