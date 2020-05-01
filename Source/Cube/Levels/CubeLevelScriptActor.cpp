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

void ACubeLevelScriptActor::SpawnCube(const FIntVector CubeId)
{
	// Spawn only unique cubes
	if (CubeList.Find(CubeId) != nullptr || CubeManagerToSpawn == nullptr)
		return;
	
	FActorSpawnParameters SpawnParams;
	const FTransform NewCubeTransform(UKismetMathLibrary::Conv_IntVectorToVector(CubeId) * 500);

	ACubeManagerActor* NewCubeManager = GetWorld()->SpawnActorDeferred<ACubeManagerActor>(CubeManagerToSpawn, NewCubeTransform);
	NewCubeManager->CubeId = CubeId;
	UGameplayStatics::FinishSpawningActor(NewCubeManager, NewCubeTransform);

	CubeList.Add(CubeId, NewCubeManager);

	// Spawn a Trap if needed
	if (CubeId != FIntVector::ZeroValue && FMath::RandRange(0, 2) > 0)
	{
		NewCubeManager->SpawnTrap();
	}
}

void ACubeLevelScriptActor::CubeInvestigated(const FIntVector CubeId)
{
	//ULog::Success("-------------------------Cube Investigated--------------------------", LO_Both);

	// If cube investigated - spawn additional cubes around the current cube (if not spawned yet)
	for (FIntVector SpawnCubeDiff : SpawnCubeList)
	{
		FIntVector NewCubeId = CubeId + SpawnCubeDiff;

		SpawnCube(NewCubeId);
	}

}

void ACubeLevelScriptActor::ReloadLevel()
{
	FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(GetWorld(), true);

	UGameplayStatics::OpenLevel(GetWorld(), TEXT("VRBasic"));

	/*AGameMode* GameMode = Cast<AGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	GameMode->ResetLevel();
	AMainCharacter* MainCharacter = Cast<AMainCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	MainCharacter->SetActorLocation(FVector(273.f, -200.f, 93.f));*/

}

