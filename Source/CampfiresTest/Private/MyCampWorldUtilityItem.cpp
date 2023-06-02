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

	// Interact icon defaults
	InteractIcon = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractIcon"));
	const FStringClassReference UtilityItemTargetClassRef(TEXT("/Game/CampfiresTest/Blueprints/UI/Interact/WBP_UtilityItemTarget.WBP_UtilityItemTarget_C"));
	InteractIcon->SetWidgetClass(UtilityItemTargetClassRef.TryLoadClass<UUserWidget>());
	InteractIcon->SetWidgetSpace(EWidgetSpace::Screen);
	InteractIcon->SetVisibility(false);
	InteractIcon->SetHiddenInGame(false);
	InteractIcon->SetRelativeLocation(FVector(0.0f, 0.0f, 150.0f));
	InteractIcon->SetupAttachment(GetRootComponent());
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
		if (CampCharacter->GetCampInteractComp()->GetCurrentUtilityItem() != this && CampCharacter->bSitting == false)
		{
			Super::Interact_Implementation(InstigatorPawn); // Calls ACampWorldItem Interact, which adds item to player's inventory.
		}
	}
}

void AMyCampWorldUtilityItem::OrientItem_Implementation(FRotator PlayerRotation)
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
		}
	}
}


