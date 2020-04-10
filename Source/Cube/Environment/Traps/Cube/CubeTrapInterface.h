// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CubeTrapInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCubeTrapInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CUBE_API ICubeTrapInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	// Check is a Trap Active
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Trap Interface")
	bool IsActive();

	// Interact with a Trap
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Trap Interface")
	void Interact();

	// Deactivate a Trap
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Trap Interface")
	void Deactivate();
};
