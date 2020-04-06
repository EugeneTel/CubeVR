// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "CubeWallActor.h"

#include "CubeManagerActor.generated.h"

UCLASS()
class CUBE_API ACubeManagerActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACubeManagerActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(VisibleAnywhere, Category = "Spawning")
	TArray<ACubeWallActor*> SpawnedWalls;

	UPROPERTY(VisibleAnywhere, Category = "Spawning")
	TArray<FTransform> SpawnWallsTransform;

	UPROPERTY(EditInstanceOnly, Category = "Spawning")
	bool bWallsSpawned;

	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	TSubclassOf<ACubeWallActor> WallToSpawn;

	UFUNCTION(BlueprintCallable)
	void SpawnCubeWalls();

};
