// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactibles/VRSliderComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"

#include "CubeDoorSliderComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class CUBE_API UCubeDoorSliderComponent : public UVRSliderComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCubeDoorSliderComponent(const FObjectInitializer& ObjectInitializer);

	virtual void TickGrip_Implementation(UGripMotionControllerComponent* GrippingController, const FBPActorGripInformation& GripInformation, float DeltaTime) override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	FTimerHandle AutoClosingTimerHandle;

protected:
	// Is Door Slider Locked
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bLocked;

public:

	// Opposite Door's Slider
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCubeDoorSliderComponent* OppositeSlider;

	// Synchronize Opposite Door's Slider progress
	UFUNCTION(BlueprintCallable)
	void SyncOppositeSlider();
	
	// Checking Door Slider Locked
	FORCEINLINE bool IsLocked() { return bLocked; }

	// Lock Door Slider
	UFUNCTION(BlueprintCallable)
	void Lock();

	// Unlock Door Slider
	UFUNCTION(BlueprintCallable)
	void Unlock();

	// Toggle Lock
	UFUNCTION(BlueprintCallable)
	void ToggleLock();

	// Automatically closing the Door Slider
	UFUNCTION(BlueprintCallable)
	void StartAutoClosing();

	// One Step For Closing
	UFUNCTION(BlueprintCallable)
	void AutoClosingStep();

	// Stop Door Auto Closing Functionality
	UFUNCTION(BlueprintCallable)
	void StopAutoClosing();
};
