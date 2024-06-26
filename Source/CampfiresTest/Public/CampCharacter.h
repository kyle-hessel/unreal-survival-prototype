// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CampCharacter.generated.h"

class AACampEquipable;
class USpringArmComponent;
class UCameraComponent;
class USphereComponent;
class UCapsuleComponent;
class UCampAttributeComponent;
class UCampActionComponent;
class UCampInteractionComponent;
class UCampInventoryComponent;
class ACampCampsite;
class ACampBackpack;
class ACampMeleeWeapon;
class ACampEnemyBase;
class ACampWorldItem;
class AMyCampWorldUtilityItem;

UCLASS()
class CAMPFIRESTEST_API ACampCharacter : public ACharacter
{
	GENERATED_BODY()

	/*
	 * Private component declarations for CampCharacter
	*/

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interact", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> ItemSphere;

	UPROPERTY(BlueprintReadOnly, Category = "Interact", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<ACampWorldItem> TargetedItem;

	UPROPERTY(BlueprintReadOnly, Category = "Interact", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<AACampEquipable> TargetedEquipable;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components | Attributes", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UCampAttributeComponent> AttributeComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components | Actions", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UCampActionComponent> ActionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components | Interactions", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UCampInteractionComponent> InteractComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components | Inventory", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UCampInventoryComponent> InventoryComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> ThirdPersonCamera;

	UPROPERTY(BlueprintReadOnly, Category = "Combat", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<ACampMeleeWeapon> ActiveMeleeWeapon;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<AActor> ActiveContainerObject;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	UCampInventoryComponent* ActiveContainerInventoryComp;

	UPROPERTY(BlueprintReadOnly, Category = "Combat", meta=(AllowPrivateAccess = "true"))
	TMap<ACampEnemyBase*, float> NearbyEnemies;

	UPROPERTY(BlueprintReadOnly, Category = "Interact", meta=(AllowPrivateAccess = "true"))
	TMap<ACampWorldItem*, float> NearbyItems;
	
	UPROPERTY(BlueprintReadOnly, Category = "Interact", meta=(AllowPrivateAccess = "true"))
	TMap<AACampEquipable*, float> NearbyEquipables; // This includes weapons as well, using equipables as a general term here contrary to collision layers, since the usage is a bit different.

	UPROPERTY(BlueprintReadOnly, Category = "Combat", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<ACampEnemyBase> LockedOnEnemy;
	
	/*
	 * Private function declarations for CampCharacter
	*/
	
	static float CompareRotRelativeToActor(const FRotator& ActorOneRotation, const FRotator& LookAtRotation);

public:
	// Sets default values for this character's properties
	ACampCharacter();

	/*
	 * Public component declarations for CampCharacter
	*/

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<USphereComponent> CombatSphere;

	/*
	 * Getters
	*/
	UFUNCTION(BlueprintPure)
	FORCEINLINE UCampAttributeComponent* GetCampAttributeComp() const { return AttributeComp; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE UCampActionComponent* GetCampActionComp() const { return ActionComp; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE UCampInteractionComponent* GetCampInteractComp() const { return InteractComp; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE UCampInventoryComponent* GetCampInventoryComp() const { return InventoryComp; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE ACampMeleeWeapon* GetActiveMeleeWeapon() const { if (ActiveMeleeWeapon) return ActiveMeleeWeapon; return nullptr; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE ACampEnemyBase* GetLockedOnEnemy() const { if (LockedOnEnemy) return LockedOnEnemy; return nullptr; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE ACampWorldItem* GetTargetedItem() const { if (TargetedItem) return TargetedItem; return nullptr; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE AACampEquipable* GetTargetedEquipable() const { if (TargetedEquipable) return TargetedEquipable; return nullptr; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE TMap<ACampEnemyBase*, float> GetNearbyEnemies() const { return NearbyEnemies; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE TMap<ACampWorldItem*, float> GetNearbyItems() const { return NearbyItems; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE TMap<AACampEquipable*, float> GetNearbyEquipables() const { return NearbyEquipables; }

	/*
	 * Setters
	*/
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetActiveMeleeWeapon(ACampMeleeWeapon* MeleeWeapon) { ActiveMeleeWeapon = MeleeWeapon; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetActiveContainer(AActor* Container) { ActiveContainerObject = Container; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetTargetedItem(ACampWorldItem* NewTarget) { TargetedItem = NewTarget; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetActiveContainerInventoryComp(UCampInventoryComponent* NewInventoryComp) { ActiveContainerInventoryComp = NewInventoryComp; }

	FORCEINLINE void RemoveNearbyEnemy(const ACampEnemyBase* Enemy) { NearbyEnemies.Remove(Enemy); }

	/*
	 * Variable declarations for CampCharacter
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	float DefaultMaxWalkSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bIsAccelerating;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bIsDecelerating;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	float AccelerationRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	float DecelerationRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float DefaultAccelerationRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float DefaultDecelerationRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	float DefaultBrakingFriction;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	float SpeedMultiplier;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	float ExhaustionMultiplier;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Time")
	float DeltaTimeScaleMultiplier;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float JumpTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bJumping;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bSitting;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bSprinting;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Access")
	bool bInAccessBox;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Access")
	bool bInteracting;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Inventory")
	bool bSortByStack;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Combat")
	bool bMeleeWeaponEquipped;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Combat")
	bool bInCombat;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Combat")
	bool bCanDrawWeapon;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Combat")
	bool bWeaponDrawn;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Combat")
	float SheathTimer;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> DrawSwordAnimation;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> SheathSwordAnimation;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> AttackChainAnimation;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bAttackPlaying;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bLockedOnToEnemy;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	float MaxEnemyLockOnToggleDistance;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	float LockOnInterpSpeed;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	float CumulativeLockOnRotDeltaTime;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bQueueNextAttackInCombo;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	int32 MeleeComboNumber;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Combat")
	int32 MaxComboNumber;

	UPROPERTY(BlueprintReadWrite, Category = "Building")
	bool bInBuildMenu;

	UPROPERTY(BlueprintReadWrite, Category = "Building")
	TArray<FVector> BuildSiteLocations;

	UPROPERTY(EditDefaultsOnly)
	float ItemSpawnDistanceFromPlayer;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Movement functions
	void MoveForward(float Value);
	void MoveRight(float Value);
	void SprintStart();
	void SprintStop();
	void Jump() override;

	void ExecuteJumpTimer(float DeltaTime);
	void ExecuteSheathTimer();

	// Interacting
	void PrimaryInteract();

	// Dropping bag, or item in hand
	void DropBag();

	// Generic place item function, used to call ToggleBuildMenu.
	void PlaceBuild();

	//UFUNCTION(BlueprintCallable, Category = "Inventory")
	//void FindItemInInventory(FName ItemName, int32 Quantity);

	// Opens inventory menu UI.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OpenInventory();

	UFUNCTION(BlueprintNativeEvent)
	void ToggleBuildMenu();

	UPROPERTY(EditDefaultsOnly, Category = "Item Classes")
	TSubclassOf<AMyCampWorldUtilityItem> FirepitClass;

	UPROPERTY(EditDefaultsOnly, Category = "Item Classes")
	TSubclassOf<AMyCampWorldUtilityItem> TentClass;

	UPROPERTY(EditDefaultsOnly, Category = "Item Classes")
	TSubclassOf<AMyCampWorldUtilityItem> TrunkClass;

	UPROPERTY(EditDefaultsOnly, Category = "Item Classes")
	TSubclassOf<AMyCampWorldUtilityItem> BenchClass;

	FTimerHandle TimerHandle_SheathTimer;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintNativeEvent, Category = "Movement")
	void Decelerate(float DeltaTime);

	// Sitting down (at campsite, for now)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Movement")
	void SitDown();

	// Standing up (at campsite, for now)
	UFUNCTION(BlueprintNativeEvent, Category = "Movement")
	void StandUp();

	UFUNCTION(BlueprintCallable, Category = "Utility")
	bool SpawnUtilityItem(const FName ItemName);

	UFUNCTION()
	void BeginCombatSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void EndCombatSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
	void BeginItemSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void EndItemSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(Category = "Interact")
	void SortNearbyItemsByDistance();

	UFUNCTION(Category = "Interact")
	void SortNearbyEquipablesByDistance();
	
	UFUNCTION(Category = "Combat")
	void SortEnemiesByDistance();

	UFUNCTION(Category = "Combat")
	bool CheckTargetVisibility(AActor* Target) const;

	UFUNCTION(Category = "Combat")
	void LockOnInputHandler();

	UFUNCTION(BlueprintNativeEvent, Category = "Combat")
	void LockOnToEnemy(ACampEnemyBase* Enemy);

	UFUNCTION(Category = "Combat")
	void TrackLockedOnEnemy(float DeltaTime, float InterpSpeed);

	UFUNCTION(BlueprintNativeEvent, Category = "Combat")
	void ReleaseLockOn(ACampEnemyBase* Enemy);

	UFUNCTION(Category = "Combat")
	void LockOnToggleBackward();

	UFUNCTION(Category = "Combat")
	void LockOnToggleForward();

	UFUNCTION(BlueprintNativeEvent, Category = "Combat")
	void PlayHitCameraShake();

	UFUNCTION(BlueprintNativeEvent, Category = "Combat")
	void PlayHitFreezeFrame(float FreezeFrameTimer, float TimeDilationFactor);

	// Attack functions
	UFUNCTION()
	void AttackStart();
	UFUNCTION()
	void AttackStop();
	UFUNCTION()
	void SheathWeapon();

};
