// Fill out your copyright notice in the Description page of Project Settings.


#include "CubeBaseTrapActor.h"

// Sets default values
ACubeBaseTrapActor::ACubeBaseTrapActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACubeBaseTrapActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACubeBaseTrapActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


bool ACubeBaseTrapActor::IsActive_Implementation()
{
	return bActive;
}

void ACubeBaseTrapActor::Activate_Implementation()
{
	return;
}
