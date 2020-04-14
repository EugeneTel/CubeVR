// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "Math/IntVector.h"
#include "Cube/Environment/Buildings/Cube/CubeManagerActor.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameMode.h"
#include "TimerManager.h"


#include "Log.h"
#include "CubeLevelScriptActor.generated.h"


/**
 * 
 */
UCLASS()
class CUBE_API ACubeLevelScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()

public:
	ACubeLevelScriptActor();

	virtual void BeginPlay() override;

	// Cube manager asset for spawning
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ACubeManagerActor> CubeManagerToSpawn;

	// Coordinates of starting cube
	UPROPERTY()
	FIntVector StartCube;

	// List of cubes which can be spawned around each other cube
	UPROPERTY()
	TArray<FIntVector> SpawnCubeList;

	// Player starts in cube with this coordinates
	UPROPERTY()
	FIntVector PlayerStartCubeId;

	// Spawned Cubes List
	UPROPERTY()
	TMap<FIntVector, class ACubeManagerActor*> CubeList;

	// Create the start cube and nearest cubes around it
	UFUNCTION(BlueprintCallable)
	void CreateStartCubes();

	// Spawn cube manager and all cube assets
	UFUNCTION(BlueprintCallable)
	void SpawnCube(FIntVector CubeId);

	// Set cube as investigated and spawn near cubes
	UFUNCTION(BlueprintCallable)
	void CubeInvestigated(FIntVector CubeId);

	// Reload current Level
	UFUNCTION(BlueprintCallable)
	void ReloadLevel();
	
};
