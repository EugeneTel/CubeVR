// Fill out your copyright notice in the Description page of Project Settings.


#include "ShoeActor.h"

// Sets default values
AShoeActor::AShoeActor(const FObjectInitializer& ObjectInitializer)
	: AGrippableStaticMeshActor(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AShoeActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AShoeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

