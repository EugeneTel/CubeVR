// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Grippables/GrippableStaticMeshComponent.h"
#include "CubeLadderComponent.generated.h"

/**
 * 
 */
UCLASS()
class CUBE_API UCubeLadderComponent : public UGrippableStaticMeshComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCubeLadderComponent(const FObjectInitializer& ObjectInitializer);
};
