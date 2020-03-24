// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VRCharacter.h"
#include "MainCharacter.generated.h"

/**
 * 
 */
UCLASS()
class CUBE_API AMainCharacter : public AVRCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hands")
	class USphereComponent* GrabSphereRight;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hands")
	class USphereComponent* GrabSphereLeft;
};
