// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Cube/Characters/MainCharacter.h"
#include "Cube/Characters/Enums/GripState.h"

#include "HandAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class CUBE_API UHandAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gripping")
    EGripState GripState;
};
