// Fill out your copyright notice in the Description page of Project Settings.


#include "CampItemTrunk.h"

#include "CampCharacter.h"
#include "CampInventoryComponent.h"

ACampItemTrunk::ACampItemTrunk()
{
	PrimaryActorTick.bCanEverTick = true;
	
	WorldItemIdentifier = 6; // Trunk

	InventoryComp = CreateDefaultSubobject<UCampInventoryComponent>(TEXT("InventoryComp"));

	LidMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LidMesh"));
	LidMesh->SetupAttachment(Item);
	
	OpenSpeed = 50.f;
}

void ACampItemTrunk::BeginPlay()
{
	Super::BeginPlay();

	//  This only stays so that chests manually placed and not spawned in the level still open and close. Otherwise it's redundant.
	ClosedTarget = LidMesh->GetComponentRotation().Roll;
	CurrentLidRoll = ClosedTarget;
	OpenTarget = LidMesh->GetComponentRotation().Roll - 55.f;
}

void ACampItemTrunk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Rotate trunk lid when player opens and closes inventory while in access box.
	if (bBeingAccessed)
	{
		if (CampCharacter->GetCampInventoryComp()->bIsOpen == true)
		{
			if (CurrentLidRoll > OpenTarget)
			{
				LidMesh->AddLocalRotation(FRotator(0.f, 0.f, -OpenSpeed * DeltaTime));
				CurrentLidRoll = LidMesh->GetComponentRotation().Roll;
			}
		}
		else
		{
			if (CurrentLidRoll < ClosedTarget)
			{
				LidMesh->AddLocalRotation(FRotator(0.f, 0.f, OpenSpeed * DeltaTime));
				CurrentLidRoll = LidMesh->GetComponentRotation().Roll;
			}
		}
	}
}

void ACampItemTrunk::Interact_Implementation(APawn* InstigatorPawn)
{
	Super::Interact_Implementation(InstigatorPawn);

	// As of now, don't do much of anything if we directly interact with the trunk while inside its access box.
	UE_LOG(LogTemp, Warning, TEXT("Trunk."));
}

void ACampItemTrunk::OrientItem_Implementation(FRotator PlayerRotation)
{
	Super::OrientItem_Implementation(PlayerRotation);

	// DO this here instead of in BeginPlay since the parent call above is what adds local rotation relative to terrain. We need this information before caching rots.
	ClosedTarget = LidMesh->GetComponentRotation().Roll;
	CurrentLidRoll = ClosedTarget;
	OpenTarget = LidMesh->GetComponentRotation().Roll - 55.f;
}

void ACampItemTrunk::BeginBoxOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::BeginBoxOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (OtherActor->IsA(ACampCharacter::StaticClass()))
	{
		CampCharacter = Cast<ACampCharacter>(OtherActor);
		CampCharacter->SetActiveContainer(this);

		UE_LOG(LogTemp, Warning, TEXT("Yeehaw."));
	}
}

void ACampItemTrunk::EndBoxOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA(ACampCharacter::StaticClass()))
	{
		CampCharacter->SetActiveContainer(nullptr);
		CampCharacter->SetActiveContainerInventoryComp(nullptr);
		CampCharacter = nullptr;
	}

	Super::EndBoxOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex);
}





