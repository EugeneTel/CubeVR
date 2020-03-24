// Fill out your copyright notice in the Description page of Project Settings.


#include "WallLadder.h"

// Sets default values
AWallLadder::AWallLadder()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AWallLadder::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWallLadder::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

