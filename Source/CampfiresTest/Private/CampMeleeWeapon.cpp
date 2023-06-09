// Fill out your copyright notice in the Description page of Project Settings.


#include "CampMeleeWeapon.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "CampCharacter.h"
#include "CampEnemyBase.h"
#include "CampEnemyAttributeComponent.h"
#include "Components/WidgetComponent.h"


// Sets default values
ACampMeleeWeapon::ACampMeleeWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Configure subobjects: mesh and necessary colliders for interaction and combat.
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetCollisionProfileName("MeleeWeapon");
	WeaponMesh->SetCollisionObjectType(ECC_GameTraceChannel2);
	SetRootComponent(WeaponMesh);

	Hitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("Hitbox"));
	Hitbox->SetupAttachment(RootComponent);
	Hitbox->SetCollisionResponseToAllChannels(ECR_Ignore);

	InteractSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractSphere"));
	InteractSphere->SetupAttachment(RootComponent);
	InteractSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	// Create overlap event bindings to functions for both the Hitbox and InteractSphere.
	Hitbox->OnComponentBeginOverlap.AddDynamic(this, &ACampMeleeWeapon::BeginHitboxOverlap);
	Hitbox->OnComponentEndOverlap.AddDynamic(this, &ACampMeleeWeapon::EndHitboxOverlap);

	InteractSphere->OnComponentBeginOverlap.AddDynamic(this, &ACampMeleeWeapon::BeginInteractSphereOverlap);
	InteractSphere->OnComponentEndOverlap.AddDynamic(this, &ACampMeleeWeapon::EndInteractSphereOverlap);

	// Item icon defaults
	Icon = CreateDefaultSubobject<UWidgetComponent>(TEXT("Icon"));
	Icon->SetWidgetSpace(EWidgetSpace::Screen);
	Icon->SetVisibility(false);
	Icon->SetHiddenInGame(false);
	Icon->SetRelativeLocation(FVector(0.0f, 0.0f, 150.0f));
	Icon->SetupAttachment(GetRootComponent());

	HolsterSocket = nullptr;
	CombatSocket = nullptr;

	bEquipped = false;
	bInCombat = false;
	
	ReceivingEnemy = nullptr;

	DefaultDamageDelta = 30.f;
	DefaultEnergyDelta = 10.f; // For consuming a bit of player energy.
	ComboTwoDmgModifier = 1.25f;
	ComboThreeDmgModifier = 1.5f;
}

// Called when the game starts or when spawned
void ACampMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACampMeleeWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACampMeleeWeapon::Interact_Implementation(APawn* InstigatorPawn)
{
	//ICampInteractionInterface::Interact_Implementation(InstigatorPawn);

	if (!bEquipped)
	{
		if (InstigatorPawn)
		{
			if (ACampCharacter* CampCharacter = Cast<ACampCharacter>(InstigatorPawn))
			{
				HolsterSocket = CampCharacter->GetMesh()->GetSocketByName("MeleeWeaponHolsterSlot");
				CombatSocket = CampCharacter->GetMesh()->GetSocketByName("MeleeWeaponCombatSlot");

				if (HolsterSocket)
				{
					// Stop player physics collision once equipped.
					WeaponMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
					CampCharacter->GetMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Ignore);
					HolsterSocket->AttachActor(this, CampCharacter->GetMesh());

					bEquipped = true;
					CampCharacter->bMeleeWeaponEquipped = bEquipped; // for animation?

					CampCharacter->SetActiveMeleeWeapon(this);
				}
			}
		}
	}
}

void ACampMeleeWeapon::BeginHitboxOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bInCombat)
	{
		ReceivingEnemy = Cast<ACampEnemyBase>(OtherActor);
		
		if (ReceivingEnemy)
		{
			// If the enemy has no health, don't do anything to it.
			if (ReceivingEnemy->GetEnemyAttributeComp()->CurrentLifeForce <= 0.0f) ReceivingEnemy = nullptr;
			
			bCollidingWithEnemy = true;
			UE_LOG(LogTemp, Warning, TEXT("GET EM"));
		}
	}
}

void ACampMeleeWeapon::EndHitboxOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (bInCombat)
	{
		ACampEnemyBase* Enemy = Cast<ACampEnemyBase>(OtherActor);

		if (ReceivingEnemy == Enemy)
		{
			bCollidingWithEnemy = false;

			ReceivingEnemy = nullptr;
		}
	}
}

void ACampMeleeWeapon::BeginInteractSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bEquipped)
	{
		
	}
}

void ACampMeleeWeapon::EndInteractSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}

