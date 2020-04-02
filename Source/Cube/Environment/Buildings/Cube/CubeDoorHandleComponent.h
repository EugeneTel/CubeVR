// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactibles/VRLeverComponent.h"
#include "Log.h"

#include "CubeDoorHandleComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class CUBE_API UCubeDoorHandleComponent : public UVRLeverComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCubeDoorHandleComponent(const FObjectInitializer& ObjectInitializer);

	virtual void TickGrip_Implementation(UGripMotionControllerComponent* GrippingController, const FBPActorGripInformation& GripInformation, float DeltaTime) override;

protected:
	// Is Door Handle Locked
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bLocked;

public:
	
	// Opposite Door Handle
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCubeDoorHandleComponent* OppositeHandle;

	FORCEINLINE bool IsLocked() { return bLocked; }

	UFUNCTION(BlueprintCallable)
	void Lock();

	UFUNCTION(BlueprintCallable)
	void Unlock();

	UFUNCTION(BlueprintCallable)
	void ToggleLock();

	// Reset Door Handle progress
	UFUNCTION(BlueprintCallable)
	void ResetProgress();
};
