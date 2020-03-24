// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WallLadder.generated.h"

UCLASS()
class CUBE_API AWallLadder : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWallLadder();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
