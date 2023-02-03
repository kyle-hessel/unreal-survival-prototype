// Fill out your copyright notice in the Description page of Project Settings.


#include "CampCharacter.h"

#include "CampActionAttack.h"
#include "CampActionComponent.h"
#include "CampAttributeComponent.h"
#include "CampEnemyBase.h"
#include "CampGameModeBase.h"
#include "CampInteractionComponent.h"
#include "CampInventoryComponent.h"
#include "CampItemBench.h"
#include "CampMeleeWeapon.h"
#include "CampWorldItem.h"
#include "MyCampWorldUtilityItem.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACampCharacter::ACampCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create the camera boom, make it a child of the root component, set its arm length, and set rotation default.
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 500.f;
	CameraBoom->bUsePawnControlRotation = true;

	// Move character independent to controller camera
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;

	// Create the camera and parent it to the default socket of the camera boom.
	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	ThirdPersonCamera->bUsePawnControlRotation = false;

	// Create the attribute component that will be attached to the player.
	AttributeComp = CreateDefaultSubobject<UCampAttributeComponent>(TEXT("AttributeComp"));

	// Create the action component that will be attached to the player.
	ActionComp = CreateDefaultSubobject<UCampActionComponent>(TEXT("ActionComp"));

	// Create the interaction component that will be attached to the player.
	InteractComp = CreateDefaultSubobject<UCampInteractionComponent>(TEXT("InteractComp"));

	// Create the inventory component that will be attached to the player.
	InventoryComp = CreateDefaultSubobject<UCampInventoryComponent>(TEXT("InventoryComp"));

	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	CombatSphere->SetupAttachment(RootComponent);
	CombatSphere->SetCollisionProfileName("Enemy");

	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &ACampCharacter::BeginCombatSphereOverlap);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &ACampCharacter::EndCombatSphereOverlap);

	// Set default acceleration rate for transitioning into sprint
	AccelerationRate = 8.0f;

	// Set default deceleration rate for transitioning out of sprint
	DecelerationRate = 12.0f;

	DefaultAccelerationRate = AccelerationRate;
	DefaultDecelerationRate = DecelerationRate;
	DefaultBrakingFriction = GetCharacterMovement()->BrakingFriction;

	// Set default slow multiplier when wearing backpack
	SpeedMultiplier = 1.0f;

	// For making values increment/decrement faster while still making use of DeltaTime
	DeltaTimeScaleMultiplier = 50.f;

	// Set default additive exhaustion multiplier from energy loss
	ExhaustionMultiplier = 1.0f;

	// Init JumpTimer and jumping bool.
	JumpTimer = 0.0f;
	bJumping = false;
	
	bSprinting = false;
	bSitting = false;
	bInBuildMenu = false;

	// Sets default inventory sorting behavior. Might not be the best place for this, but better than on the InventoryComp since this should be a global setting for all inventories, not each individual one.
	bSortByStack = true;

	// Used for accessing other inventories besides our own or our backpack's.
	ActiveContainerObject = nullptr;

	// Used for combat
	ActiveMeleeWeapon = nullptr;
	bAttackPlaying = false;
	bCanDrawWeapon = true;
	bWeaponDrawn = false;
	SheathTimer = 2.f;
	bLockedOnToEnemy = false;
	MaxEnemyLockOnToggleDistance = 700.f; // Specifically for toggling lock on with ToggleLeft and ToggleRight actions, makes toggle system a bit smarter. See respective functions.
	LockOnInterpSpeed = 0.f;
	CumulativeLockOnRotDeltaTime = 0.f;
	bQueueNextAttackInCombo = false;
	MeleeComboNumber = 0;
	MaxComboNumber = 3;
	LockedOnEnemy = nullptr;

	ItemSpawnDistanceFromPlayer = 160.f;
}

// Called when the game starts or when spawned
void ACampCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Give the player the ability to attack.
	ActionComp->AddAction(UCampActionAttack::StaticClass());

	// Cache our initial max walk speed that we set in the editor before playtime for use in our Sprint Action
	DefaultMaxWalkSpeed = GetMovementComponent()->GetMaxSpeed();
}

// Called every frame
void ACampCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// If decelerating flag is set to true, call the Decelerate each function each frame until it disables the flag.
	if (bIsDecelerating == true) Decelerate(DeltaTime); // Implemented as a blueprint event but called from C++.

	// Check JumpTimer every frame.
	ExecuteJumpTimer(DeltaTime);

	// If we are locked onto the enemy, track their location.
	if (bLockedOnToEnemy)
	{
		TrackLockedOnEnemy(DeltaTime, LockOnInterpSpeed);
	}
	// If we drop our lock-on, reset accumulated DeltaTime.
	else
	{
		if (CumulativeLockOnRotDeltaTime != 0.f) CumulativeLockOnRotDeltaTime = 0.f;
	}
}

// Called to bind functionality to input
void ACampCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Bind MoveForward and MoveRight axis inputs to their respective functions within this class.
	PlayerInputComponent->BindAxis("MoveForward", this, &ACampCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACampCharacter::MoveRight);

	// Bind SprintStart and SprintStop to action input for pressing and releasing the Sprint key.
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ACampCharacter::SprintStart);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ACampCharacter::SprintStop);

	// Bind Turn and LookUp axis inputs to functions we've inherited from APawn that change controller pitch and yaw.
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	// Bind Jump action input depending on if the input is being pressed and released to functions we've inherited from ACharacter.
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACampCharacter::Jump); // Use our child override of this function for added jump functionality
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind Attack action input, to initiate attack state or continue attack combo.
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &ACampCharacter::AttackStart);

	// Bind Sheath action input, to put away an active weapon and end combat state.
	PlayerInputComponent->BindAction("Sheath", IE_Pressed, this, &ACampCharacter::SheathWeapon);

	// Bind ToggleLockOn action input to determine what enemy to lock onto, or to drop lock-on.
	PlayerInputComponent->BindAction("ToggleLockOn", IE_Pressed, this, &ACampCharacter::LockOnInputHandler);

	// Bind interact action to PrimaryInteract function, which in turn will call CampInteractionComponent, which in turn will execute an implementation of ICampInteractionInterface's Interact function.
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ACampCharacter::PrimaryInteract);

	// Bind drop interaction to DropItem function, which calls DropEquipable from the Interact Component.
	PlayerInputComponent->BindAction("Drop", IE_Pressed, this, &ACampCharacter::DropBag);

	// Bind place action to PlaceItem function. Places typically a campsite, but potentially other items such as sleeping bags.
	PlayerInputComponent->BindAction("Place", IE_Pressed, this, &ACampCharacter::PlaceBuild);

	// Bind toggle left and right actions to their respective functions, this is for combat lock-on toggling.
	PlayerInputComponent->BindAction("ToggleLeft", IE_Pressed, this, &ACampCharacter::LockOnToggleBackward);
	PlayerInputComponent->BindAction("ToggleRight", IE_Pressed, this, &ACampCharacter::LockOnToggleForward);

	// **** This binding to call the right function is now done in blueprint instead of C++, because Unreal's C++ API makes it nearly fucking impossible to access key information in a usable way.
	// Some inventory logic can still be implemented in C++ through use of BlueprintNativeEvent as I was doing already.
	//PlayerInputComponent->BindAction("Inventory", IE_Pressed, this, &ACampCharacter::OpenInventory);
}

void ACampCharacter::MoveForward(float Value)
{
	//FVector Forward = GetActorForwardVector();
	
	if (InventoryComp->bIsOpen == false && bInteracting == false)
	{
		// Get controller rotation and zero out the Pitch and Roll; we only need the Yaw (rotation around Z-axis). Also make sure we aren't sitting.
		if (Controller && Value != 0.0f && !bSitting)
		{
			FRotator ControlRot = GetControlRotation();
			ControlRot.Pitch = 0.0f;
			ControlRot.Roll = 0.0f;
			const FVector ForwardVector = ControlRot.Vector();
		
			/* Convert ControlRot to a vector and make this our direction; the view direction is now the direction that 
			 * the character will run in, but we are still decoupled from strictly rotating with the controller.
			 * This allows for us to move the camera around and not the character in some situations.
			 */
			AddMovementInput(ForwardVector, Value);
		}
	}
}

void ACampCharacter::MoveRight(float Value)
{
	//FVector Right = GetActorRightVector();

	if (InventoryComp->bIsOpen == false && bInteracting == false)
	{
		if (Controller && Value != 0.0f && !bSitting)
		{
			FRotator ControlRot = GetControlRotation();
			ControlRot.Pitch = 0.0f;
			ControlRot.Roll = 0.0f;

			// Get our Y (right vector) of our Player Controller's rotation matrix.
			const FVector RightVector = FRotationMatrix(ControlRot).GetScaledAxis(EAxis::Y);
		
			// Use RightVector to influence sideways movement input relative to the controller's direction.
			AddMovementInput(RightVector, Value);
		}
	}
}

// Start sprinting through our attached ActionComponent. This creates a new instantiation of CampAction.
void ACampCharacter::SprintStart()
{
	if (InventoryComp->bIsOpen == false && bInteracting == false)
	{
		// Don't start action if we are sitting.
		if (!bSitting)
		{
			bSprinting = true;
	
			const float MovementSpeedMagnitude = GetVelocity().Length();

			// Require that we are already moving to start sprinting, otherwise the player can hold still and 'accelerate.' There might be a more elegant way to do this.
			if (MovementSpeedMagnitude > 0.0f)
			{
				// Create a new action on our ActionComponent with us (CampCharacter) as the instigator, and an action name of Sprint.
				ActionComp->StartActionByName(this, "Sprint", true);
			}
		}
	}
}

// Stop sprinting through our attached ActionComponent.
void ACampCharacter::SprintStop()
{
	// Stop the Sprint action and pass through us (CampCharacter) as the instigator.
	ActionComp->StopActionByName(this, "Sprint");
}

// Override of built-in Character jump method.
void ACampCharacter::Jump()
{
	if (InventoryComp->bIsOpen == false && bInteracting == false)
	{
		// Only execute the parent function call if our JumpTimer isn't above 0. This effectively disables jumping when the timer is decrementing upon landing.
		if (JumpTimer <= 0.0f && !bSitting)
		{
			Super::Jump();
			UE_LOG(LogTemp, Warning, TEXT("yump!"));

			// Set bJumping to true to begin JumpTimer after successfully jumping.
			bJumping = true;
		}
	}
}

// Add nearby enemies to an array of enemies for combat tracking.
void ACampCharacter::BeginCombatSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACampEnemyBase* NearbyEnemy = Cast<ACampEnemyBase>(OtherActor);

	// ***Add a toggle for auto lock-on later, as it may not be desirable for some players.
	if (NearbyEnemy)
	{
		// Don't add dead enemies.
		if (NearbyEnemy->bDead == false)
		{
			if (bInCombat == true)
			{
				const bool bEnemyVisible = CheckTargetVisibility(OtherActor);
			
				// Lock onto an enemy, this logic handles if there aren't any nearby enemies yet, implying we aren't locked onto any.
				if (NearbyEnemies.IsEmpty() && bEnemyVisible == true)
				{
					LockOnToEnemy(NearbyEnemy);
				}
			}
		
			NearbyEnemies.Add(NearbyEnemy);
			UE_LOG(LogTemp, Warning, TEXT("Nearby enemy detected."))
		}
	}
}

// Remove enemy from array of enemies for combat tracking if they go outside the sphere.
void ACampCharacter::EndCombatSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ACampEnemyBase* ExitingEnemy = Cast<ACampEnemyBase>(OtherActor);

	if (ExitingEnemy)
	{
		if (ExitingEnemy->bDead == false)
		{
			if (LockedOnEnemy)
			{
				// If ExitingEnemy is the only enemy in the array as they leave, stop locking on to anything.
				if (NearbyEnemies.Num() == 1)
				{
					ReleaseLockOn(LockedOnEnemy);
				}
				// If there are multiple enemies, check if this enemy is the locked-onto enemy and release lock if so.
				else
				{
					if (ExitingEnemy == LockedOnEnemy)
					{
						ReleaseLockOn(LockedOnEnemy);

						// After dropping lock-on, lock-on to the nearest enemy to the player.
						SortEnemiesByDistance();

						CumulativeLockOnRotDeltaTime = 0.f;

						// Convert keys of NearbyEnemies TMap into an array so we can directly access values.
						TArray<ACampEnemyBase*> NearbyEnemiesKeys;
						NearbyEnemies.GenerateKeyArray(NearbyEnemiesKeys);
			
						// Lock onto the nearest enemy, which will always be the first in the array when this function is called as a result of the above calculations.
						LockOnToEnemy(NearbyEnemiesKeys[0]);
					}
				}
			}
		
			NearbyEnemies.Remove(ExitingEnemy);
			UE_LOG(LogTemp, Warning, TEXT("Nearby enemy lost."))
		}
	}
}

void ACampCharacter::SortEnemiesByDistance()
{
	TArray<ACampEnemyBase> NearbyEnemiesSortedByDistance;
	
	// Sort through the TMap of enemy Keys and populate all of their Values with their current distance from the player.
	for (auto& Enemy : NearbyEnemies)
	{
		FVector EnemyLocLateral = FVector(Enemy.Key->GetActorLocation().X, Enemy.Key->GetActorLocation().Y, 0.f);
		FVector PlayerLocLateral = FVector(GetActorLocation().X, GetActorLocation().Y, 0.f);
		FVector DistanceToEnemyVec = EnemyLocLateral - PlayerLocLateral;
		float DistanceToEnemy = DistanceToEnemyVec.Length();
		UE_LOG(LogTemp, Display, TEXT("Distance to enemy: %f"), DistanceToEnemy);

		Enemy.Value = DistanceToEnemy;
	}

	// Sort the TMap of enemies by distance to player from lowest to highest.
	NearbyEnemies.ValueSort([](const float A, const float B)
	{
		return A < B;
	});

	// *** DEBUG ONLY, comment out later
	for (auto& Enemy : NearbyEnemies)
	{
		UE_LOG(LogTemp, Display, TEXT("New enemy distance: %f"), Enemy.Value);
	}
}

// Sweep from both the camera to the target and the player to the target, and if either stores a HitResult we know there is an object in the way. Returns false if it hits something.
bool ACampCharacter::CheckTargetVisibility(AActor* Target) const
{
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	
	FHitResult Hit;
	bool bBlockingHit = GetWorld()->LineTraceSingleByObjectType(Hit, ThirdPersonCamera->GetComponentLocation(), Target->GetActorLocation(), ObjectQueryParams);
	//DrawDebugLine(GetWorld(), ThirdPersonCamera->GetComponentLocation(), Target->GetActorLocation(), FColor::Blue, false, 3.0f, 0, 1.5f);

	if (bBlockingHit)
	{
		if (!Hit.GetActor()->IsA(APawn::StaticClass()))
		{
			UE_LOG(LogTemp, Display, TEXT("Obstructions detected."));
			return false;
		}
	}

	FHitResult HitTwo;
	bool bBlockingHitTwo = GetWorld()->LineTraceSingleByObjectType(HitTwo, GetActorLocation(), Target->GetActorLocation(), ObjectQueryParams);
	//DrawDebugLine(GetWorld(), GetActorLocation(), Target->GetActorLocation(), FColor::Red, false, 3.0f, 0, 1.5f);

	if (bBlockingHitTwo)
	{
		if (!HitTwo.GetActor()->IsA(APawn::StaticClass()))
		{
			UE_LOG(LogTemp, Display, TEXT("Obstructions detected."));
			return false;
		}
	}

	UE_LOG(LogTemp, Display, TEXT("No obstructions."));
	// If both lines only collided with a pawn, return true.
	return true;
}

// Called when the ToggleLockOn input binding is executed, and determines if we lock-on or release lock-on. Separate from auto-lock.
void ACampCharacter::LockOnInputHandler()
{
	// Only toggle lock if there are nearby enemies.
	if (NearbyEnemies.Num() > 0)
	{
		// If we have a locked-on enemy, release the lock.
		if (bLockedOnToEnemy == true)
		{
			ReleaseLockOn(LockedOnEnemy);
		}
		// If we do not have a locked-on enemy, figure out which enemy to lock onto.
		else
		{
			if (NearbyEnemies.Num() > 1)
			{
				// Sort all enemies by distance at this exact moment to determine which we lock onto initially (the closest).
				SortEnemiesByDistance();
			}

			// Convert keys of NearbyEnemies TMap into an array so we can directly access values.
			TArray<ACampEnemyBase*> NearbyEnemiesKeys;
			NearbyEnemies.GenerateKeyArray(NearbyEnemiesKeys);

			AActor* NearestEnemyActor = Cast<AActor>(NearbyEnemiesKeys[0]);
			const bool bEnemyVisible = CheckTargetVisibility(NearestEnemyActor);

			if (bEnemyVisible)
			{
				// Lock onto the nearest enemy, which will always be the first in the array when this function is called as a result of the above calculations.
				LockOnToEnemy(NearbyEnemiesKeys[0]);
			}
		}
	}
}

// Locks onto the given enemy. Extra logic in BP regarding character movement.
void ACampCharacter::LockOnToEnemy_Implementation(ACampEnemyBase* Enemy)
{
	LockedOnEnemy = Enemy;

	if (LockedOnEnemy)
	{
		// Get the rotation of the player looking at the locked on enemy.
		const FRotator NewLookRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), LockedOnEnemy->GetActorLocation());

		// Compare the player's current rotation to the above rotation to see if they are close to one another or not.
		const float RotRelativeToEnemy = CompareRotRelativeToActor(GetActorRotation(), NewLookRotation);

		// Use the above two calculations to determine how fast we should rotate the player to face the enemy: slower when facing away, faster when facing closer.
		if (RotRelativeToEnemy <= 0.2f)
		{
			LockOnInterpSpeed = 0.2f;
			UE_LOG(LogTemp, Display, TEXT("Fast interp lock-on"));
		}
		else if (RotRelativeToEnemy > 0.2f && RotRelativeToEnemy <= 0.6f)
		{
			LockOnInterpSpeed = 0.08f;
			UE_LOG(LogTemp, Display, TEXT("Normal interp lock-on"));
		}
		else if (RotRelativeToEnemy > 0.6f)
		{
			LockOnInterpSpeed = 0.025f;
			UE_LOG(LogTemp, Display, TEXT("Slow interp lock-on"));
		}
		else
		{
			// If somehow none of these are true, stick with a middle value.
			LockOnInterpSpeed = 0.4f;
		}

		bLockedOnToEnemy = true; // Do this last, as the above calculations need to happen first to determine interp speed before tick calls TrackLockedOnEnemy repeatedly.
		LockedOnEnemy->bLockedOn = bLockedOnToEnemy;
		LockedOnEnemy->LockOnReticle->SetHiddenInGame(false);
	}
}

// Releases lock on from given enemy. Extra logic in BP regarding character movement.
void ACampCharacter::ReleaseLockOn_Implementation(ACampEnemyBase* Enemy)
{
	bLockedOnToEnemy = false;
	LockedOnEnemy->bLockedOn = bLockedOnToEnemy;
	LockedOnEnemy->LockOnReticle->SetHiddenInGame(true);
	LockedOnEnemy = nullptr;
}

// Tracks the locked onto enemy.
void ACampCharacter::TrackLockedOnEnemy(float DeltaTime, float InterpSpeed)
{
	if (LockedOnEnemy)
	{
		const FVector EnemyLocation = LockedOnEnemy->GetActorLocation();
		
		// Orient the player to the enemy.
		const FRotator NewLookRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), EnemyLocation);
		constexpr float SwordCorrectionModifier = 6.f; // Slight correction on the Yaw to accomodate for the player's sword angle with current animations. May need tweaking later or depending on anim.
		const FRotator NewLookRotationCorrected = FRotator(GetActorRotation().Pitch, NewLookRotation.Yaw + SwordCorrectionModifier, GetActorRotation().Roll);
		// **** Still getting some weird behavior with RInterpTo and this exact Delta Time value, needs further investigation.
		constexpr float DeltaCorrection = 0.2f; // This value is surprisingly important, raising it to this amount seems to have helped mitigate weird RInterpTo behavior?
		CumulativeLockOnRotDeltaTime += (DeltaCorrection + DeltaTime); // Accumulate DeltaTime for interpolation function below, otherwise rot new rot will never be reached.
		const FRotator InterpNewLookRot = UKismetMathLibrary::RInterpTo(GetActorRotation(), NewLookRotationCorrected, CumulativeLockOnRotDeltaTime, InterpSpeed); // Smoothly transition to facing enemy.
		const FRotator NewActorRotation = FRotator(GetActorRotation().Pitch, InterpNewLookRot.Yaw, GetActorRotation().Roll); // Only use the Yaw from interpolation calculation.
		SetActorRotation(NewActorRotation);
		
		// Orient the camera to the enemy. Make this toggleable on and off in the settings later.
		const FVector CameraLocation = ThirdPersonCamera->GetComponentLocation();
		const FRotator NewCamLookRotation = UKismetMathLibrary::FindLookAtRotation(CameraLocation, EnemyLocation);
		const FRotator NewCamLookRotationCorrected = FRotator(-25.f, NewCamLookRotation.Yaw, NewCamLookRotation.Roll);
		// Literally no idea why GetWorldDeltaSeconds works here but not in the above RInterpTo.
		const FRotator InterpNewCameraLookRot = UKismetMathLibrary::RInterpTo(GetController()->GetControlRotation(), NewCamLookRotationCorrected, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), 10.f);
		const FRotator NewCameraRot = FRotator(InterpNewCameraLookRot.Pitch, InterpNewCameraLookRot.Yaw, ThirdPersonCamera->GetComponentRotation().Roll);
		GetController()->SetControlRotation(NewCameraRot);
	}
	
}

void ACampCharacter::LockOnToggleBackward()
{
	if (bLockedOnToEnemy && NearbyEnemies.Num() > 1)
	{
		// Calculate nearest enemy to toggle to backward using the player's look rotation at the locked on enemy relative to the player's look rotation to every other enemy.
		const FVector LockedOnEnemyLocation = LockedOnEnemy->GetActorLocation();
		const FRotator PlayerLookRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), LockedOnEnemyLocation);
		
		for (auto& Enemy : NearbyEnemies)
		{
			FVector EnemyLocation = Enemy.Key->GetActorLocation();
			const FRotator PlayerLookRotationAtThisEnemy = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), EnemyLocation);
			// Set the enemy's value in the TMap to this rotation comparison.
			Enemy.Value = FVector::DotProduct(PlayerLookRotation.Vector(), PlayerLookRotationAtThisEnemy.Vector());

			UE_LOG(LogTemp, Display, TEXT("Ratio to enemy: %f"), Enemy.Value);
		}

		// Sort the TMap of enemies by ratio of angle to player from highest to lowest. The currently selected enemy should always be first in this map.
		NearbyEnemies.ValueSort([](const float A, const float B)
		{
			return A > B;
		});

		// *** DEBUG ONLY, comment out later
		for (auto& Enemy : NearbyEnemies)
		{
			UE_LOG(LogTemp, Display, TEXT("New enemy ratio: %f"), Enemy.Value);
		}

		// Convert keys of NearbyEnemies TMap into an array so we can directly access values.
		TArray<ACampEnemyBase*> NearbyEnemiesKeys;
		NearbyEnemies.GenerateKeyArray(NearbyEnemiesKeys);

		// **** This next bit feels a bit overtly expensive for what it's doing and I may just shrink the CombatSphere instead lol
		TArray<ACampEnemyBase*> EnemiesToTarget;

		// Strip this temporary array of any enemies more than a certain amount of units away, even if they are considered nearby enemies. This stops locking onto far away enemies you can't even hit when shuffling.
		for (int32 Pos = 0; Pos < NearbyEnemiesKeys.Num() - 1; Pos++)
		{
			FVector EnemyLoc = NearbyEnemiesKeys[Pos]->GetActorLocation(); // I'm keeping Z in the calculation as well to help with not targeting enemies too far above the player.
			FVector PlayerLoc = GetActorLocation();
			FVector DistanceToEnemyVec = EnemyLoc - PlayerLoc;
			const float DistanceToEnemy = DistanceToEnemyVec.Length();

			if (DistanceToEnemy < MaxEnemyLockOnToggleDistance)
			{
				// I do it this way with yet another temporary array because I seem to get errors removing entries from this array WHILE iterating through it using both types of for loops.
				EnemiesToTarget.Add(NearbyEnemiesKeys[Pos]);
			}
		}

		AActor* EnemyActor = Cast<AActor>(EnemiesToTarget.Last());
		const bool bEnemyVisible = CheckTargetVisibility(EnemyActor);

		if (bEnemyVisible)
		{
			if (EnemiesToTarget.Num() > 1)
			{
				CumulativeLockOnRotDeltaTime = 0.f;
				
				ReleaseLockOn(LockedOnEnemy);
				LockOnToEnemy(EnemiesToTarget.Last());
			}
		}
	}
}

void ACampCharacter::LockOnToggleForward()
{
	if (bLockedOnToEnemy && NearbyEnemies.Num() > 1)
	{
		// Calculate nearest enemy to toggle to forward using the player's look rotation at the locked on enemy relative to the player's look rotation to every other enemy.
		const FVector LockedOnEnemyLocation = LockedOnEnemy->GetActorLocation();
		const FRotator PlayerLookRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), LockedOnEnemyLocation);
		
		for (auto& Enemy : NearbyEnemies)
		{
			FVector EnemyLocation = Enemy.Key->GetActorLocation();
			const FRotator PlayerLookRotationAtThisEnemy = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), EnemyLocation);
			// Set the enemy's value in the TMap to this rotation comparison.
			Enemy.Value = FVector::DotProduct(PlayerLookRotation.Vector(), PlayerLookRotationAtThisEnemy.Vector());

			UE_LOG(LogTemp, Display, TEXT("Ratio to enemy: %f"), Enemy.Value);
		}

		// Sort the TMap of enemies by ratio of angle to player from highest to lowest. The currently selected enemy should always be first in this map.
		NearbyEnemies.ValueSort([](const float A, const float B)
		{
			return A > B;
		});

		// *** DEBUG ONLY, comment out later
		for (auto& Enemy : NearbyEnemies)
		{
			UE_LOG(LogTemp, Display, TEXT("New enemy ratio: %f"), Enemy.Value);
		}

		// Convert keys of NearbyEnemies TMap into an array so we can directly access values.
		TArray<ACampEnemyBase*> NearbyEnemiesKeys;
		NearbyEnemies.GenerateKeyArray(NearbyEnemiesKeys);

		// **** This next bit feels a bit overtly expensive for what it's doing and I may just shrink the CombatSphere instead lol
		TArray<ACampEnemyBase*> EnemiesToTarget;

		// Strip this temporary array of any enemies more than a certain amount of units away, even if they are considered nearby enemies. This stops locking onto far away enemies you can't even hit when shuffling.
		for (int32 Pos = 0; Pos < NearbyEnemiesKeys.Num() - 1; Pos++)
		{
			FVector EnemyLoc = NearbyEnemiesKeys[Pos]->GetActorLocation(); // I'm keeping Z in the calculation as well to help with not targeting enemies too far above the player.
			FVector PlayerLoc = GetActorLocation();
			FVector DistanceToEnemyVec = EnemyLoc - PlayerLoc;
			const float DistanceToEnemy = DistanceToEnemyVec.Length();

			if (DistanceToEnemy < MaxEnemyLockOnToggleDistance)
			{
				// I do it this way with yet another temporary array because I seem to get errors removing entries from this array WHILE iterating through it using both types of for loops.
				EnemiesToTarget.Add(NearbyEnemiesKeys[Pos]);
			}
		}

		AActor* EnemyActor = Cast<AActor>(EnemiesToTarget[1]);
		const bool bEnemyVisible = CheckTargetVisibility(EnemyActor);

		if (bEnemyVisible)
		{
			if (EnemiesToTarget.Num() > 1)
			{
				CumulativeLockOnRotDeltaTime = 0.f;
				
				ReleaseLockOn(LockedOnEnemy);
				LockOnToEnemy(EnemiesToTarget[1]);
			}
		}
	}
}

// Implemented in blueprint for easy setup, but called in C++. In BP it invokes BP_CombatCameraShake, which is derived from CampDefaultCameraShakeBase.
void ACampCharacter::PlayHitCameraShake_Implementation()
{
}

// Helper function. Meant to return a value from 0-1 comparing the actor's given rotation to its 'look at rotation'—its rotation when looking at another given actor.
// Use UKismetMathLibrary::FindLookAtRotation from ActorOne to ActorTwo, and plug that in as LookAtRotation. ActorOneRotation will be GetActorRotation() from that actor.
float ACampCharacter::CompareRotRelativeToActor(FRotator ActorOneRotation, FRotator LookAtRotation)
{
	const float RotDot = FVector::DotProduct(LookAtRotation.Vector(), ActorOneRotation.Vector()); // Get dot product to compare both rotations turned into vectors
	const float AddedLengths = LookAtRotation.Vector().Length() + ActorOneRotation.Vector().Length(); // Add the length of both vectors together
	const float CalculatedDifference = (1.f - (RotDot / AddedLengths)) - 0.5f; // Divide the dot product of both vectorized rotations by the length of said vectorized rotations added together, and offset the value to move it into a 0-1 range.
	
	UE_LOG(LogTemp, Warning, TEXT("Rotation difference: %f"), CalculatedDifference);

	return CalculatedDifference;
}

// Implemented in blueprint, called from CampAnimNotifyStateMeleeAttack.
void ACampCharacter::PlayHitFreezeFrame_Implementation(float FreezeFrameTimer, float TimeDilationFactor)
{
}

// Starts the ability to attack by drawing a sword, the attack itself is handled in the attack action.
void ACampCharacter::AttackStart()
{
	if (InventoryComp->bIsOpen == false && !bSitting && bCanDrawWeapon)
	{
		if (ActiveMeleeWeapon)
		{
			// If not in combat, activate combat.
			if (ActiveMeleeWeapon->bInCombat == false)
			{
				ActiveMeleeWeapon->bInCombat = true;
				bInCombat = ActiveMeleeWeapon->bInCombat; // for animation
				ActiveMeleeWeapon->GetHitbox()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
				
				ActionComp->StartActionByName(this, "Attack", false);
			}
			// If in combat, decide if combo animation is continued.
			else
			{
				// Only begin attack animation if weapon is already drawn and in the proper slot (hand).
				if (bWeaponDrawn)
				{
					// If we left click while playing an attack, allow the next attack in the combo to play if there is one.
					if (bAttackPlaying == true)
					{
						bQueueNextAttackInCombo = true;
					}
					else
					{
						if (bQueueNextAttackInCombo == false)
						{
							// If we left click while idling in the attack state with sword out, begin the attack animation.
							PlayAnimMontage(AttackChainAnimation);
							if (MeleeComboNumber < 1) MeleeComboNumber++;
							UE_LOG(LogTemp, Warning, TEXT("Combo number: %d"), MeleeComboNumber);
						}
					}
				}
			}
		}
	}
}

// End attack action.
void ACampCharacter::AttackStop()
{
	if (ActiveMeleeWeapon)
	{
		ActiveMeleeWeapon->bInCombat = false;
		ActiveMeleeWeapon->GetHitbox()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		bInCombat = ActiveMeleeWeapon->bInCombat;
		
		ActionComp->StopActionByName(this, "Attack");
	}
}

void ACampCharacter::SheathWeapon()
{
	if (InventoryComp->bIsOpen == false && bInCombat == true)
	{
		if (bAttackPlaying == false && bQueueNextAttackInCombo == false)
		{
			PlayAnimMontage(SheathSwordAnimation);
			AttackStop();

			// Set that our melee weapon can no longer be drawn, and use a timer to set this back to true after a set duration.
			bCanDrawWeapon = false;
			GetWorldTimerManager().SetTimer(TimerHandle_SheathTimer, this, &ACampCharacter::ExecuteSheathTimer, SheathTimer);
		}
	}	
}


// Jump timer implementation. Stops both bunny-hopping and animations breaking as a result of that.
void ACampCharacter::ExecuteJumpTimer(float DeltaTime)
{
	const float JumpTimerIncrement = (DeltaTime * DeltaTimeScaleMultiplier) * 1.5f;
	
	//UE_LOG(LogTemp, Warning, TEXT("JumpTimer: %f"), JumpTimer);
	
	// Only increment the timer if we are jumping (in midair).
	if (bJumping)
	{
		JumpTimer += JumpTimerIncrement;

		// Once we hit the ground, update bJumping to false.
		if (!GetMovementComponent()->IsFalling()) bJumping = false;
	}
	// If our JumpTimer has been incremented by any value and we aren't in midair, begin decrementing the timer so that we can jump again.
	if (JumpTimer > 0.0f && !GetMovementComponent()->IsFalling())
	{
		JumpTimer -= JumpTimerIncrement * 2; // Decrement at twice the rate we increment.
	}
	// If JumpTimer goes below 0.0 due to different increment and decrement values, set it back to 0.
	if (JumpTimer < 0.0f) JumpTimer = 0.0f;
}

// Once sheath timer is up, let the player draw their weapon again.
void ACampCharacter::ExecuteSheathTimer()
{
	bCanDrawWeapon = true;
}

// Call the attached CampInteractionComponent's PrimaryInteract function when the interact key is pressed.
void ACampCharacter::PrimaryInteract()
{
	if (InventoryComp->bIsOpen == false && bInBuildMenu == false)
	{
		// (might) want to set this to only trigger if we aren't jumping, unsure as of now.
		if (InteractComp) InteractComp->PrimaryInteract();
	}
}

// Call the attached CampInteractionComponent's DropInteractable function when the drop key is pressed.
void ACampCharacter::DropBag()
{
	if (InventoryComp->bIsOpen == false)
	{
		// Only allow for dropping bags when we aren't jumping.
		if (JumpTimer <= 0.0f)
		{
			if (InteractComp) InteractComp->DropEquipable();
		}
	}
}

// Called from the Place action mapping.
void ACampCharacter::PlaceBuild()
{
	if (InventoryComp->bIsOpen == false && bInteracting == false && bSitting == false)
	{
		if (JumpTimer <= 0.0f)
		{
			// This function should soon open an item creation context menu.

			ToggleBuildMenu();
			bInBuildMenu = !bInBuildMenu;
		}
	}
}

// Configured in Blueprint, spawns a widget blueprint.
void ACampCharacter::ToggleBuildMenu_Implementation()
{
}

//void ACampCharacter::FindItemInInventory_Implementation(FName ItemName, int32 Quantity)
//{
//}

// Opens inventory. Handled in blueprint since setting up UI is a pain in C++. All to be done in C++ is set ActiveContainerInventoryComp if there is an ActiveContainerObject.
void ACampCharacter::OpenInventory_Implementation()
{
	if (ActiveContainerObject)
	{
		if (ActiveContainerInventoryComp == nullptr)
		{
			// *** A ton of blueprint logic occurs after what is written below - see BP_CampCharacter's Event Open Inventory.
			UActorComponent* FetchedInventoryComponent = ActiveContainerObject->GetComponentByClass(UCampInventoryComponent::StaticClass());

			ActiveContainerInventoryComp = Cast<UCampInventoryComponent>(FetchedInventoryComponent);
		}
		else
		{
			ActiveContainerInventoryComp = nullptr;
		}
	}
}

// Blueprint callable function for spawning utility items from inventory.
bool ACampCharacter::SpawnUtilityItem(const FName ItemName)
{

	// Do a line trace downwards to determine if the item can be placed.
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);

	if (ItemName == FName(TEXT("Firepit"))) ItemSpawnDistanceFromPlayer = 180.f;
	else if (ItemName == FName(TEXT("Tent"))) ItemSpawnDistanceFromPlayer = 220.f;
	else if (ItemName == FName(TEXT("Trunk"))) ItemSpawnDistanceFromPlayer = 150.f;
	else if (ItemName == FName(TEXT("Bench"))) ItemSpawnDistanceFromPlayer = 140.f;

	FVector ActorHeightAdjustment = FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + 50.f);
	FVector LateralLoc = ActorHeightAdjustment + GetActorForwardVector() * ItemSpawnDistanceFromPlayer;

	float ActorLocZOffset = LateralLoc.Z - 200.f;
	
	FHitResult Hit;
	FVector LineTraceEnd = FVector(LateralLoc.X, LateralLoc.Y, ActorLocZOffset);

	bool bBlockingHit = GetWorld()->LineTraceSingleByObjectType(Hit, LateralLoc, LineTraceEnd, ObjectQueryParams);
	DrawDebugLine(GetWorld(), LateralLoc, LineTraceEnd, FColor::Silver, false, 3.0f, 0, 1.5f);

	if (bBlockingHit)
	{
		FRotator GroundAlignedRotation = UKismetMathLibrary::MakeRotFromZX(Hit.ImpactNormal, LateralLoc);
		
		FActorSpawnParameters SpawnParams;

		FTransform SpawnTransform = FTransform(GroundAlignedRotation, Hit.Location);
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		TArray<FName> RowNames = Items->GetRowNames();
		ACampWorldItem* SpawnedItem;

		// Determine which subclass of camp world utility item to spawn, and if item isn't one, spawn a normal camp world item.
		// Using TSubClassOf variables here that are set in the editor so that functionality from both C++ and blueprint extended versions of the below classes is used.
		if (ItemName == FName(TEXT("Firepit"))) // Sadly can't do a switch statement on FNames, apparently. If converted to an FString, I could.
		{
			SpawnedItem = GetWorld()->SpawnActor<AMyCampWorldUtilityItem>(FirepitClass, SpawnTransform, SpawnParams);
		}
		else if (ItemName == FName(TEXT("Tent")))
		{
			SpawnedItem = GetWorld()->SpawnActor<AMyCampWorldUtilityItem>(TentClass, SpawnTransform, SpawnParams);
		}
		else if (ItemName == FName(TEXT("Trunk")))
		{
			SpawnedItem = GetWorld()->SpawnActor<AMyCampWorldUtilityItem>(TrunkClass, SpawnTransform, SpawnParams);
		}
		else if (ItemName == FName(TEXT("Bench")))
		{
			SpawnedItem = GetWorld()->SpawnActor<AMyCampWorldUtilityItem>(BenchClass, SpawnTransform, SpawnParams);
		}
		else
		{
			SpawnedItem = GetWorld()->SpawnActor<ACampWorldItem>(ACampWorldItem::StaticClass(), SpawnTransform, SpawnParams);
		}
	
		const FItemStruct* NewItemData = Items->FindRow<FItemStruct>(ItemName, "Create", true);

		SpawnedItem->ItemID = NewItemData->ItemID;
		SpawnedItem->WorldItemIdentifier = NewItemData->ItemID;
		SpawnedItem->WorldItemName = ItemName;
		SpawnedItem->DisplayName = NewItemData->DisplayName;
		SpawnedItem->DisplayDescription = NewItemData->DisplayDescription;
		SpawnedItem->bIsStackable = NewItemData->bIsStackable;
		SpawnedItem->Item->SetStaticMesh(NewItemData->Mesh);
		SpawnedItem->ItemType = NewItemData->ItemType;
		SpawnedItem->bEnergyItem = NewItemData->bEnergyItem;
		SpawnedItem->EnergyDelta = NewItemData->EnergyDelta;
		SpawnedItem->bLifeForceItem = NewItemData->bLifeForceItem;
		SpawnedItem->LifeForceDelta = NewItemData->LifeForceDelta;

		FVector OriginalActorLocation = SpawnedItem->GetActorLocation();
		// Positions the newly spawned item, based on its own implementation of the below interface.
		ICampItemPlacementInterface::Execute_PlaceItem(SpawnedItem, OriginalActorLocation, GetActorRotation());

		UE_LOG(LogTemp, Warning, TEXT("Spawned new item."));

		return true;
	}

	return false;
}

// Sit the character down at the current campsite's bench. Also implemented in blueprint to call this functionality plus killing movement.
void ACampCharacter::SitDown_Implementation()
{
	if (bInCombat == false && NearbyEnemies.Num() == 0)
	{
		InteractComp->SetCurrentSittingItem(Cast<ACampItemBench>(InteractComp->GetCurrentUtilityItem()));
		const ACampItemBench* SittingItem = InteractComp->GetCurrentSittingItem();
		
		// Change pawn collision when sitting.
		SittingItem->Item->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		
		// Fetch the new bench's location so we can place the player on it.
		const FVector SitLocation = SittingItem->Item->GetSocketLocation("BenchSitLocation");
		const FRotator SitRotation = SittingItem->Item->GetSocketRotation("BenchSitLocation");

		// Tweaking the SitLocation so that the player's capsule component is offset and their mesh aligns properly.
		//const FVector SitLocationAdjusted = SitLocation + (SittingItem->Item->GetForwardVector() * 62) - (SittingItem->Item->GetRightVector() * 5);

		// Place CampCharacter on the bench.
		SetActorLocation(SitLocation);
		SetActorRotation(SitRotation);
		
		// Set that we are sitting to play the right animations and disable movement.
		bSitting = true;
		UE_LOG(LogTemp, Warning, TEXT("Sat down."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemies nearby, unable to sit."));
	}
}

// Stand the character back up. Also implemented in blueprint to reset braking fiction on CampCharacter's movement component.
void ACampCharacter::StandUp_Implementation()
{
	if (InventoryComp->bIsOpen == false)
	{
		// Reset collision between player and bench.
		InteractComp->GetCurrentSittingItem()->Item->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

		InteractComp->SetCurrentSittingItem(nullptr);
				
		// Reorient the player's rotation.
		const FRotator CurrentCharacterRotation = GetActorRotation();
		const FRotator RealignedRotation = FRotator(0.f, CurrentCharacterRotation.Yaw, 0.f);
		SetActorRotation(RealignedRotation);

		bSitting = false;
	}
}

// Decelerate the player out of sprinting only. Implemented in blueprint.
void ACampCharacter::Decelerate_Implementation(float DeltaTime)
{
	
}
