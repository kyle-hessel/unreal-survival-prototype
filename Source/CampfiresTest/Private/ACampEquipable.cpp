// Fill out your copyright notice in the Description page of Project Settings.


#include "ACampEquipable.h"
#include "Components/WidgetComponent.h"

// Sets default values
AACampEquipable::AACampEquipable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AACampEquipable::BeginPlay()
{
	Super::BeginPlay();

	Icon->SetWidgetClass(IconClass);
}

// Called every frame
void AACampEquipable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

