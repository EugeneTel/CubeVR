// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "Math/IntVector.h"
#include "Cube/Environment/Buildings/Cube/CubeManagerActor.h"
#include "Kismet/KismetMathLibrary.h"


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

	UPROPERTY(EditAnywhere, Category = "Spawn")
	TSubclassOf<class ACubeManagerActor> CubeManagerToSpawn;

	UPROPERTY()
	FIntVector StartCube;

	// List of cubes which can be spawned around each other cube
	UPROPERTY()
	TArray<FIntVector> SpawnCubeList;

	UPROPERTY()
	FIntVector PlayerStartCubeId;

	UPROPERTY()
	TMap<FIntVector, class ACubeManagerActor*> CubeList;

	UFUNCTION()
	void CreateStartCubes();

	UFUNCTION()
	void SpawnCube(FIntVector CubeId);

	UFUNCTION()
	void CubeInvestigated(FIntVector CubeId);


	
};
