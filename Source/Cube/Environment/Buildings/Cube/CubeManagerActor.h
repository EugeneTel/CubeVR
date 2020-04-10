// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "CubeWallActor.h"
#include "Cube/Levels/CubeLevelScriptActor.h"
#include "Math/UnrealMathUtility.h"
#include "Cube/Environment/Traps/Cube/CubeBaseTrapActor.h"

#include "CubeManagerActor.generated.h"

USTRUCT()
struct CUBE_API FCubeData
{
	GENERATED_BODY()

public:

	UPROPERTY()
	UMaterialInterface* GlassMaterial;

	UPROPERTY()
	FColor LightColor;
};

UENUM()
enum class ECubeColor : uint8
{
	ECC_White,
	ECC_Blue,
	ECC_Green,
	ECC_Red,

	ECC_MAX
};

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

	// Cache components
	virtual void PostInitializeComponents() override;

	UPROPERTY()
	class ACubeLevelScriptActor* LevelScript;

	// If the cube investigated by player
	UPROPERTY()
	bool bInvestigated;

	// Called when player investigate the cube
	UFUNCTION()
	void Investigate();

	UPROPERTY()
	FIntVector CubeId;

	UPROPERTY()
	FCubeData CubeData;

	UPROPERTY()
	ECubeColor CubeColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TArray<UMaterialInterface*> GlassMaterialList;

	UPROPERTY(VisibleAnywhere, Category = "Spawning")
	TArray<class ACubeWallActor*> SpawnedWalls;

	UPROPERTY(VisibleAnywhere, Category = "Spawning")
	TArray<FTransform> SpawnWallsTransform;

	UPROPERTY(EditInstanceOnly, Category = "Spawning")
	bool bWallsSpawned;

	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	TSubclassOf<ACubeWallActor> WallToSpawn;

	// Spawn All cube wall actors
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Spawning")
	void SpawnCubeWalls();

	// Destroy Spawned wall actors
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Spawning")
	void DestroyCubeWalls();

	// Spawned Trap in the Cube
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	ACubeBaseTrapActor* Trap;

	// Trap List for spawning
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TArray<TSubclassOf<ACubeBaseTrapActor>> TrapList;

	// Spawn a Trap for the Cube
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Spawning")
	void SpawnTrap();

	// Destroy spawned trap
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Spawning")
	void DestroyTrap();
};
