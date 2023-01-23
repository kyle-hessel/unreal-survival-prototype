// Fill out your copyright notice in the Description page of Project Settings.


#include "CampEnemyBase.h"

#include "CampEnemyAttributeComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"

// Sets default values
ACampEnemyBase::ACampEnemyBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AggroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AggroSphere"));
	AggroSphere->SetupAttachment(RootComponent);

	AttackSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AttackSphere"));
	AttackSphere->SetupAttachment(RootComponent);
	AttackSphere->SetCollisionObjectType(ECC_GameTraceChannel3);

	// For stopping other pawns from getting too close during combat, or in general.
	BoundsCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("BoundsCapsule"));
	BoundsCapsule->SetupAttachment(AttackSphere);
	BoundsCapsule->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoundsCapsule->SetCollisionObjectType(ECC_Pawn);
	BoundsCapsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BoundsCapsule->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

	LockOnReticle = CreateDefaultSubobject<UWidgetComponent>(TEXT("LockOnReticle"));
	LockOnReticle->SetupAttachment(RootComponent);
	LockOnReticle->SetHiddenInGame(true);

	AttributeComp = CreateDefaultSubobject<UCampEnemyAttributeComponent>(TEXT("AttributeComp"));

	bLockedOn = false;
	bDead = false;
}

// Called when the game starts or when spawned
void ACampEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACampEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called when this enemy dies. Ragdolling is handled in CampAnimNotifyRagdoll, which is triggered in the necessary anim montage.
void ACampEnemyBase::Die()
{
	PlayAnimMontage(DieAnimation);
	bDead = true;

	// Disable collisions with the player once the enemy is dead.
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	BoundsCapsule->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	// Mark actor for deletion.
	SetLifeSpan(10.0f);
}
