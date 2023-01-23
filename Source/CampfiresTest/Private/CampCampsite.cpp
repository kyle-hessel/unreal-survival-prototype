// Fill out your copyright notice in the Description page of Project Settings.


#include "CampCampsite.h"
#include "CampCharacter.h"
#include "CampInventoryComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/InputSettings.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"

ACampCampsite::ACampCampsite()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create the inventory component that will be attached to the campsite.
	InventoryComp = CreateDefaultSubobject<UCampInventoryComponent>(TEXT("InventoryComp"));

	BoundingBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoundingBox"));
	BoundingBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SetRootComponent(BoundingBox);

	CampfirePit = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CampfirePit"));
	CampfirePit->SetupAttachment(RootComponent);
	CampfirePit->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	CampfirePit->SetCollisionObjectType(ECC_WorldStatic);

	Campfire = CreateDefaultSubobject<UParticleSystem>(TEXT("Campfire"));

	Tent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tent"));
	Tent->SetupAttachment(RootComponent);
	Tent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	Tent->SetCollisionObjectType(ECC_WorldStatic);

	Bench = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bench"));
	Bench->SetupAttachment(RootComponent);
	Bench->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	Bench->SetCollisionObjectType(ECC_WorldStatic);

	Trunk = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Trunk"));
	Trunk->SetupAttachment(RootComponent);
	Trunk->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	Trunk->SetCollisionObjectType(ECC_WorldStatic);

	TrunkAccessBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TrunkAccessBox"));
	TrunkAccessBox->SetupAttachment(Trunk);
	TrunkAccessBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	// When something overlaps the trunk access box, begin an overlap event.
	TrunkAccessBox->OnComponentBeginOverlap.AddDynamic(this, &ACampCampsite::BeginTrunkOverlap);
	TrunkAccessBox->OnComponentEndOverlap.AddDynamic(this, &ACampCampsite::EndTrunkOverlap);

	bWithinTrunkRange = false;
}

void ACampCampsite::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACampCampsite::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// When overlapping with a trunk's access box, do the following.
void ACampCampsite::BeginTrunkOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (CampCharacter && OtherActor->IsA(ACampCharacter::StaticClass()))
	{
		bWithinTrunkRange = true;
		
		CampCharacter->SetActiveContainer(this);
		
		UE_LOG(LogTemp, Warning, TEXT("Now within trunk range."));
	}
}

// When ending overlap with a trunk's access box, do the following.
void ACampCampsite::EndTrunkOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (CampCharacter && OtherActor->IsA(ACampCharacter::StaticClass()))
	{
		bWithinTrunkRange = false;

		CampCharacter->SetActiveContainer(nullptr);

		UE_LOG(LogTemp, Warning, TEXT("Now leaving trunk range."));
	}
}

// Decide what happens when the player interacts with the campsite.
void ACampCampsite::Interact_Implementation(APawn* InstigatorPawn)
{
	//ICampInteractionInterface::Interact_Implementation(InstigatorPawn);

	//if (InstigatorPawn) CampCharacter = Cast<ACampCharacter>(InstigatorPawn);

	if (CampCharacter)
	{
		UE_LOG(LogTemp, Display, TEXT("campsite interacted bruh!!"));

		// May be worth refactoring this in the future to use a custom player collision channel, in theory  enemies, etc. could pass through while this is active.
		if (CampCharacter->bSitting)
		{
			// Ignore the player's capsule component if sitting.
			Bench->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		}
		else
		{
			Bench->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NOOOOOOOOOOOOOOOOOOOO"));
	}
}

// Might want to add an end overlap event for CampCampsite that clears the CampCharacter pointer when the character leaves. Ensure that bounding box for the entire site extends past all components.
