// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CampTree.generated.h"

class UCapsuleComponent;
class UInstancedStaticMeshComponent;
class UHierarchicalInstancedStaticMeshComponent;

UCLASS()
class CAMPFIRESTEST_API ACampTree : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACampTree();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UCapsuleComponent> CapsuleCollider;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tree", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInstancedStaticMeshComponent> TrunkMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tree", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UHierarchicalInstancedStaticMeshComponent> LeavesMesh;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
