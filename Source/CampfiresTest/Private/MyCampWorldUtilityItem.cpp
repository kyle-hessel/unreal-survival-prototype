// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCampWorldUtilityItem.h"

#include "CampCharacter.h"
#include "CampGameModeBase.h"
#include "CampInteractionComponent.h"
#include "CampWorldItem.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"

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
	
	if (const ACampCharacter* CampCharacter = Cast<ACampCharacter>(InstigatorPawn))
	{
		// If this utility item isn't the player's current utility item (meaning we're not in the access box) just pick it up.
		// Probably would be faster here to just check if bInAccessBox is true instead of checking the utility item itself. Not sure if there's a reason I did it this way.
		if (CampCharacter->GetCampInteractComp()->GetCurrentUtilityItem() != this && CampCharacter->bSitting == false)
		{
			Super::Interact_Implementation(InstigatorPawn); // Calls ACampWorldItem Interact, which adds item to player's inventory.
		}

		if (CampCharacter->bInAccessBox)
		{
			Icon->SetVisibility(false);
		}
	}
}

void AMyCampWorldUtilityItem::OrientItem_Implementation(const FRotator PlayerRotation)
{
	ICampItemPlacementInterface::OrientItem_Implementation(PlayerRotation);
	
	const float RotationDifference = PlayerRotation.Yaw - GetActorRotation().Yaw;
	AddActorLocalRotation(FRotator(0.f, RotationDifference + 90.f, 0.f));
}

void AMyCampWorldUtilityItem::BeginBoxOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(ACampCharacter::StaticClass()))
	{
		ACampCharacter* CampCharacter = Cast<ACampCharacter>(OtherActor);
		// this check helps ensure only one access box at a time without overwriting one another
		// might need another look, still isn't perfect.
		if (CampCharacter->bInAccessBox == false)
		{
			CampCharacter->bInAccessBox = true;
			bBeingAccessed = true;
			CampCharacter->GetCampInteractComp()->SetCurrentUtilityItem(this);
			
			Icon->SetWidgetClass(InteractIconClass);
		}
	}
}

void AMyCampWorldUtilityItem::EndBoxOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA(ACampCharacter::StaticClass()))
	{
		ACampCharacter* CampCharacter = Cast<ACampCharacter>(OtherActor);
		// // this check helps ensure only one access box at a time without clearing one another
		// might need another look, still isn't perfect.
		if (CampCharacter->bInAccessBox == true)
		{
			CampCharacter->bInAccessBox = false;
			bBeingAccessed = false;
			CampCharacter->GetCampInteractComp()->SetCurrentUtilityItem(nullptr);
			
			Icon->SetWidgetClass(IconClass);
			
			if (CampCharacter->GetTargetedItem() == this)
			{
				Icon->SetVisibility(true);
			}
		}
	}
}


