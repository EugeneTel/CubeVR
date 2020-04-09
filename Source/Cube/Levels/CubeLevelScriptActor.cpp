// Fill out your copyright notice in the Description page of Project Settings.


#include "CubeLevelScriptActor.h"

ACubeLevelScriptActor::ACubeLevelScriptActor()
{

	StartCube = FIntVector(0, 0, 0);
	// Prepare start cubes list
	SpawnCubeList.Add(FIntVector(0, 0, -1));
	SpawnCubeList.Add(FIntVector(0, 0, 1));
	SpawnCubeList.Add(FIntVector(0, -1, 0));
	SpawnCubeList.Add(FIntVector(0, 1, 0));
	SpawnCubeList.Add(FIntVector(-1, 0, 0));
	SpawnCubeList.Add(FIntVector(1, 0, 0));

	PlayerStartCubeId = FIntVector(0, 0, 0);

}

void ACubeLevelScriptActor::BeginPlay()
{
	//ULog::Success("-------------------------BeginPlay--------------------------", LO_Both);

	CreateStartCubes();
}

void ACubeLevelScriptActor::CreateStartCubes()
{
	SpawnCube(StartCube);
	CubeInvestigated(StartCube);
}

void ACubeLevelScriptActor::SpawnCube(FIntVector CubeId)
{
	// Spawn only unique cubes
	if (CubeList.Find(CubeId) != nullptr || CubeManagerToSpawn == nullptr)
		return;
	
	FActorSpawnParameters SpawnParams;
	FVector NewCubeLocation = UKismetMathLibrary::Conv_IntVectorToVector(CubeId) * 500;

	ACubeManagerActor* NewCubeManager = GetWorld()->SpawnActor<ACubeManagerActor>(CubeManagerToSpawn, NewCubeLocation, FRotator::ZeroRotator, SpawnParams);
	NewCubeManager->CubeId = CubeId;
	NewCubeManager->SpawnTrap();

	CubeList.Add(CubeId, NewCubeManager);

	//ULog::Success("-------------------------Cube Created--------------------------", LO_Both);
	//ULog::Vector(NewCubeLocation, LO_Both);
}

void ACubeLevelScriptActor::CubeInvestigated(FIntVector CubeId)
{
	//ULog::Success("-------------------------Cube Investigated--------------------------", LO_Both);

	// If cube investigated - spawn additional cubes around the current cube (if not spawned yet)
	for (FIntVector SpawnCubeDiff : SpawnCubeList)
	{
		FIntVector NewCubeId = CubeId + SpawnCubeDiff;
		SpawnCube(NewCubeId);
	}

}

