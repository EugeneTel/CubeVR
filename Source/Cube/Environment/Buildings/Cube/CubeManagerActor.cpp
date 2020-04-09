// Fill out your copyright notice in the Description page of Project Settings.


#include "CubeManagerActor.h"

// Sets default values
ACubeManagerActor::ACubeManagerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bWallsSpawned = false;
	bInvestigated = false;

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

	SpawnCubeWalls();
}

// Called every frame
void ACubeManagerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACubeManagerActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void ACubeManagerActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	LevelScript = Cast<ACubeLevelScriptActor>(GetWorld()->GetLevelScriptActor());
}

void ACubeManagerActor::Investigate()
{
	if (bInvestigated)
		return;

	bInvestigated = true;
	LevelScript->CubeInvestigated(CubeId);
}

void ACubeManagerActor::SpawnCubeWalls()
{
	if (WallToSpawn == nullptr || bWallsSpawned)
		return;

	if (SpawnedWalls.Num() > 0)
	{
		SpawnedWalls.Empty();
	}

	FActorSpawnParameters SpawnParams;
	
	int32 MaterialIndex = FMath::RandRange(0, GlassMaterialList.Num() - 1);
	UMaterialInterface* GlassMaterial = GlassMaterialList[MaterialIndex];

	for (FTransform WallTransform : SpawnWallsTransform)
	{
		ACubeWallActor* SpawnedWall = GetWorld()->SpawnActor<ACubeWallActor>(
			WallToSpawn, 
			GetActorLocation() + WallTransform.GetLocation(), 
			GetActorRotation() + WallTransform.GetRotation().Rotator(), 
			SpawnParams
		);
		SpawnedWall->SetGlassMaterial(GlassMaterial);
		SpawnedWall->Manager = this;
		SpawnedWall->FindOppositeWall();

		SpawnedWalls.Add(SpawnedWall);
	}

	bWallsSpawned = true;
}

void ACubeManagerActor::SpawnTrap()
{

	// Spawn a Trap if needed
	if (CubeId == FIntVector::ZeroValue || TrapList.Num() == 0 || FMath::RandRange(0, 2) == 0)
		return;

	ULog::Success("TRAP GENERATED");

	int32 TrapIndex = FMath::RandRange(0, TrapList.Num() - 1);

	FActorSpawnParameters SpawnParams;

	Trap = GetWorld()->SpawnActor<ACubeBaseTrapActor>(
		TrapList[TrapIndex],
		GetActorLocation(),
		GetActorRotation(),
		SpawnParams
	);
}
