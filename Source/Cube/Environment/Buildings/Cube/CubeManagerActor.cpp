// Fill out your copyright notice in the Description page of Project Settings.


#include "CubeManagerActor.h"

// Sets default values
ACubeManagerActor::ACubeManagerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bWallsSpawned = true;

	SpawnWallsTransform.Add(FTransform(FRotator::ZeroRotator, FVector::ZeroVector));
	SpawnWallsTransform.Add(FTransform(FRotator(-90.0f , 174.28f, -264.28f), FVector(0.f, 200.f, 200.f)));
	SpawnWallsTransform.Add(FTransform(FRotator(-90.0f, -14.03f, -255.96f), FVector(0.f, -200.f, 200.f)));
	SpawnWallsTransform.Add(FTransform(FRotator(-90.0f, 0.f, 0.f), FVector(-200.f, 0.f, 200.f)));
	SpawnWallsTransform.Add(FTransform(FRotator(-90.0f, -6.72f, -173.27f), FVector(200.f, 0.f, 200.f)));
	SpawnWallsTransform.Add(FTransform(FRotator(0.f, 0.f, -180.f), FVector(0.f, 0.f, 400.f)));
}

// Called when the game starts or when spawned
void ACubeManagerActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACubeManagerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACubeManagerActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	SpawnCubeWalls();
}

void ACubeManagerActor::SpawnCubeWalls()
{
	if (WallToSpawn == nullptr || bWallsSpawned)
		return;

	FActorSpawnParameters SpawnParams;

	for (FTransform WallTransform : SpawnWallsTransform)
	{
		SpawnedWalls.Add(GetWorld()->SpawnActor<ACubeWallActor>(WallToSpawn, GetActorLocation() + WallTransform.GetLocation(), GetActorRotation() + WallTransform.GetRotation().Rotator(), SpawnParams));
	}

	bWallsSpawned = true;
}

