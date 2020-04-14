// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CubeBaseTrapActor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "Cube/Characters/MainCharacter.h"
#include "Kismet/GameplayStatics.h"

#include "Log.h"
#include "CubeSpikesTrapActor.generated.h"

/**
 * 
 */
UCLASS()
class CUBE_API ACubeSpikesTrapActor : public ACubeBaseTrapActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACubeSpikesTrapActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Interact_Implementation() override;

	virtual void Deactivate_Implementation() override;

	UFUNCTION()
	void OnColliderOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSpikesOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Spawn all spikes for the trap
	UFUNCTION(BlueprintCallable)
	void SpawnSpikes();

	// Spawn a Instance Static Mesh
	UFUNCTION(BlueprintCallable)
	void SpawnInstMesh(FName MeshName, FTransform MeshTransform);

	// Root component for all Instanced Meshes
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* InstMeshRootComponent;

	// Trap collider for activation
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UBoxComponent* Collider;

	// A List of Instance Meshes for each wall
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FTransform> SpawnInstMeshList;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<UInstancedStaticMeshComponent*> InstMeshList;

	// A list with transform instances on each Instance Mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FTransform> InstTransformList;

	// Progress of Showing Spikes
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float Progress;

	// TimerHandle for activation/deactivation progress
	UPROPERTY()
	FTimerHandle ProgressTimerHandle;

	// When Active starts ticking and increase showing progress
	UFUNCTION(BlueprintCallable)
	void ActivationProgress();

	// Indicates deactivation process
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bDeactivate;

	// When not active starts ticking and hides spikes
	UFUNCTION(BlueprintCallable)
	void DeactivationProgress();

	UFUNCTION(BlueprintCallable)
	void CheckCharacterHit();
};
