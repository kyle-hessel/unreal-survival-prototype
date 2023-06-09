// Fill out your copyright notice in the Description page of Project Settings.


#include "CampBackpack.h"

#include "CampAttributeComponent.h"
#include "CampCharacter.h"
#include "CampInventoryComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/SkeletalMeshSocket.h"

// Sets default values
ACampBackpack::ACampBackpack()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create the inventory component that will be attached to the backpack, separate from the player's inventory.
	InventoryComp = CreateDefaultSubobject<UCampInventoryComponent>(TEXT("InventoryComp"));
	
	// Configure the backpack's OffsetBox (this is what we actually put on the player)
	OffsetBox = CreateDefaultSubobject<UBoxComponent>(TEXT("OffsetBox"));
	SetRootComponent(OffsetBox);
	OffsetBox->SetCollisionProfileName("Equipable");
	OffsetBox->SetCollisionObjectType(ECC_GameTraceChannel1);
	OffsetBox->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	OffsetBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OffsetBox->SetGenerateOverlapEvents(true);
	// Configure the backpack's mesh (this is what the player sees)
	BackpackMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BackpackMesh"));
	BackpackMesh->SetupAttachment(OffsetBox);
	BackpackMesh->SetCollisionProfileName("Equipable"); // Our custom Equipable profile, in the event we need to block against certain things later
	BackpackMesh->SetCollisionObjectType(ECC_GameTraceChannel1); // This is 'Equipable' in the editor's collision menu: check defaultengine.ini to verify.
	BackpackMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	BackpackMesh->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore); // Specifically setting this so that only the offset box is considered for item targeting.
	BackpackMesh->SetGenerateOverlapEvents(true);

	// Item icon defaults
	Icon = CreateDefaultSubobject<UWidgetComponent>(TEXT("Icon"));
	Icon->SetWidgetSpace(EWidgetSpace::Screen);
	Icon->SetVisibility(false);
	Icon->SetHiddenInGame(false);
	Icon->SetRelativeLocation(FVector(0.0f, 0.0f, 150.0f));
	Icon->SetupAttachment(GetRootComponent());

	EnergyExpenditureModifier = 1.75f; // Bigger backpacks should have a higher value for this.
	PreviousEnergyDelta = 0.0f; // Just a default.
	bEquipped = false;
}

// Called when the game starts or when spawned
void ACampBackpack::BeginPlay()
{
	Super::BeginPlay();
}

// Trigger an event when the player (or something else) interacts with the backpack
void ACampBackpack::Interact_Implementation(APawn* InstigatorPawn)
{
	//ICampInteractionInterface::Interact_Implementation(InstigatorPawn);

	if (InstigatorPawn) CampCharacter = Cast<ACampCharacter>(InstigatorPawn);

	// Get the location to place the backpack off of CampCharacter, and set our actor's location equal to that socket.
	if (CampCharacter)
	{
		// Set our backpack mesh to ignore pawns when equipped and our character to ignore equipables while holding one
		BackpackMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		CampCharacter->GetMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);

		// When we equip our bag, increase the rate at which we burn through energy.
		if (UCampAttributeComponent* AttributeComp = CampCharacter->GetCampAttributeComp())
		{
			PreviousEnergyDelta = AttributeComp->DefaultEnergyDelta; // Also store previous value.
			AttributeComp->DefaultEnergyDelta *= EnergyExpenditureModifier;
		}

		bEquipped = true; // Mark that this backpack is equipped. Used for some checks in other places.


		// Store CampCharacter's BackpackSlot socket in a temporary variable and attach this actor to it.
		if (const USkeletalMeshSocket* BackpackSocket = CampCharacter->GetMesh()->GetSocketByName("BackpackSlot"))
		{
			BackpackSocket->AttachActor(this, CampCharacter->GetMesh());
		}

		UE_LOG(LogTemp, Warning, TEXT("Backpack equipped!"));
	}
}

// Called every frame
void ACampBackpack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

/* Drop backpack and determine where to place it in the world, don't drop if there is no viable location (e.g. edge of a cliff, player facing away).
 * 
 * This function is also implemented in blueprint so that additional functionality can be called right before this to access the player's
 * movement component and reset its MaxWalkSpeed and SpeedMultiplier values so that speed returns to normal as well.
*/
bool ACampBackpack::DropBackpack_Implementation()
{
	if (CampCharacter)
	{
		// Move our bag's location so that it does not touch the player anymore but stays behind them; do this right before changing collision channels back.
		FVector NewBagLocation = BackpackMesh->GetComponentLocation() - CampCharacter->GetActorForwardVector() * 12.f;

		// Set up our line trace query parameters to look for static world objects
		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);

		float ZTraceDistance = 200.f;

		// Set how far downwards we send the line to look for the ground
		FVector BagLocationDownwards = FVector(NewBagLocation.X, NewBagLocation.Y, NewBagLocation.Z - ZTraceDistance);
		
		FHitResult Hit;
		// Do a line trace downwards until we hit the ground to decide how far down to place the backpack
		bool bBlockingHit = GetWorld()->LineTraceSingleByObjectType(Hit, NewBagLocation, BagLocationDownwards, ObjectQueryParams);

		DrawDebugLine(GetWorld(), NewBagLocation, BagLocationDownwards, FColor::Yellow, false, 3.0f, 0, 1.5f);

		// If we hit something, drop the backpack.
		if (bBlockingHit)
		{
			// Detach backpack from player, keep existing world transform.
			DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			
			// Mark bag as unequipped.
			if (bEquipped) bEquipped = false;
			UE_LOG(LogTemp, Warning, TEXT("Dropped backpack!"));

			// Set an offset so the backpack appears above-ground.
			float BackpackZOffset = 28.f; //** This is going to need modification depending on backpack size I think.
			FVector BackpackLocationCorrected = FVector(Hit.ImpactPoint.X, Hit.ImpactPoint.Y, Hit.ImpactPoint.Z + BackpackZOffset);
			
			SetActorLocation(BackpackLocationCorrected);

			// Zero out the Pitch and Roll for the bag's rotation, just keep the Z axis rotation from DetachFromActor and flip it.
			FRotator NewBagRotation = FRotator(0.f, GetActorRotation().Yaw, 0.f);
			SetActorRotation(NewBagRotation);
		
			// Set collision profiles back to their original states on both the backpack mesh and the character's mesh so that the two are in agreement.
			BackpackMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
			CampCharacter->GetMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);

			// Set the character's energy consumption delta back to normal.
			if (UCampAttributeComponent* AttributeComp = CampCharacter->GetCampAttributeComp())
			{
				AttributeComp->DefaultEnergyDelta = PreviousEnergyDelta;
			}

			return true;
		}

		// If we did not hit something, return false.
		UE_LOG(LogTemp, Warning, TEXT("Could not drop backpack."));

		return false;

	}
	return false;
}

