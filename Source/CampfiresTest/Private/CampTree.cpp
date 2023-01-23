// Fill out your copyright notice in the Description page of Project Settings.


#include "CampTree.h"

#include "Components/CapsuleComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"

// Sets default values
ACampTree::ACampTree()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleCollider"));
	CapsuleCollider->SetCollisionResponseToAllChannels(ECR_Ignore);
	CapsuleCollider->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	CapsuleCollider->SetupAttachment(GetRootComponent());
	CapsuleCollider->SetMobility(EComponentMobility::Static);
	
	TrunkMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("TrunkMesh"));
	TrunkMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	TrunkMesh->SetupAttachment(CapsuleCollider);
	TrunkMesh->SetMobility(EComponentMobility::Static);

	LeavesMesh = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("LeavesMesh"));
	LeavesMesh->SetupAttachment(TrunkMesh);
	LeavesMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	LeavesMesh->SetMobility(EComponentMobility::Movable);
	LeavesMesh->SetCullDistances(40000, 50000);
}

// Called when the game starts or when spawned
void ACampTree::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACampTree::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

