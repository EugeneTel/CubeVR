// Fill out your copyright notice in the Description page of Project Settings.


#include "CubeFireTrapActor.h"

// Sets default values
ACubeFireTrapActor::ACubeFireTrapActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACubeFireTrapActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACubeFireTrapActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


