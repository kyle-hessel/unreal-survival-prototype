// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCampWorldUtilityItem.h"

#include "CampCharacter.h"
#include "CampGameModeBase.h"
#include "CampInteractionComponent.h"
#include "CampWorldItem.h"
#include "Components/BoxComponent.h"

AMyCampWorldUtilityItem::AMyCampWorldUtilityItem()
{
	ItemType = EItemType::ITEM_Tool;

	Item->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Item->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

	AccessBox = CreateDefaultSubobject<UBoxComponent>(TEXT("AccessBox"));
	AccessBox->SetupAttachment(Item);
	AccessBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	AccessBox->OnComponentBeginOverlap.AddDynamic(this, &AMyCampWorldUtilityItem::BeginBoxOverlap);
	AccessBox->OnComponentEndOverlap.AddDynamic(this, &AMyCampWorldUtilityItem::EndBoxOverlap);
}

void AMyCampWorldUtilityItem::BeginPlay()
{
	Super::BeginPlay();
}

void AMyCampWorldUtilityItem::Interact_Implementation(APawn* InstigatorPawn)
{
	UE_LOG(LogTemp, Warning, TEXT("CampWorldUtilityItem"));

	// Spawn an interact menu that lets the player either interact with the item in a meaningful way, OR pick it up.
	if (const ACampCharacter* CampCharacter = Cast<ACampCharacter>(InstigatorPawn))
	{
		if (CampCharacter->bInAccessBox == false && CampCharacter->bSitting == false)
		{
			Super::Interact_Implementation(InstigatorPawn); // Calls ACampWorldItem Interact, which adds item to player's inventory.
		}
	}
}

void AMyCampWorldUtilityItem::PlaceItem_Implementation(FVector SpawnLocation, FRotator PlayerRotation)
{
	ICampItemPlacementInterface::PlaceItem_Implementation(SpawnLocation, PlayerRotation);

	const FVector HeightAdjustment = Item->GetStaticMesh()->GetBoundingBox().GetSize();

	SetActorLocation(FVector(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z + HeightAdjustment.Z * 0.5f));
	const float RotationDifference = PlayerRotation.Yaw - GetActorRotation().Yaw;
	AddActorLocalRotation(FRotator(0.f, RotationDifference, 0.f));
}

void AMyCampWorldUtilityItem::BeginBoxOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(ACampCharacter::StaticClass()))
	{
		ACampCharacter* CampCharacter = Cast<ACampCharacter>(OtherActor);
		CampCharacter->bInAccessBox = true;
		CampCharacter->GetCampInteractComp()->SetCurrentUtilityItem(this);
	}
}

void AMyCampWorldUtilityItem::EndBoxOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA(ACampCharacter::StaticClass()))
	{
		ACampCharacter* CampCharacter = Cast<ACampCharacter>(OtherActor);
		CampCharacter->bInAccessBox = false;
		CampCharacter->GetCampInteractComp()->SetCurrentUtilityItem(nullptr);
	}
}


