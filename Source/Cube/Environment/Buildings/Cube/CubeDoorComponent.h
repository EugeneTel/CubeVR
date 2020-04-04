// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactibles/VRSliderComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "CubeDoorHandleComponent.h"

#include "CubeDoorComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class CUBE_API UCubeDoorComponent : public UVRSliderComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCubeDoorComponent(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void OnGrip_Implementation(UGripMotionControllerComponent* GrippingController, const FBPActorGripInformation& GripInformation) override;
	virtual void OnGripRelease_Implementation(UGripMotionControllerComponent* ReleasingController, const FBPActorGripInformation& GripInformation, bool bWasSocketed) override;

	virtual void TickGrip_Implementation(UGripMotionControllerComponent* GrippingController, const FBPActorGripInformation& GripInformation, float DeltaTime) override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void OnChildGrip_Implementation(UGripMotionControllerComponent* GrippingController, const FBPActorGripInformation& GripInformation) override;
	virtual void OnChildGripRelease_Implementation(UGripMotionControllerComponent* ReleasingController, const FBPActorGripInformation& GripInformation, bool bWasSocketed) override;

private:
	FTimerHandle StepAutoClosingTimerHandle;
	FTimerHandle InitAutoClosingTimerHandle;

protected:
	// Is Door Locked
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bLocked;

	FTimerManager* WorldTimerManager;

public:

	// Time in seconds before the door starts closing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cube")
	float DoorAutoClosingTime;

	// Door handle component with static mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cube")
	UCubeDoorHandleComponent* HandleComponent;

	// Opposite Door from the nearest cube
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCubeDoorComponent* OppositeDoor;

	UFUNCTION(BlueprintCallable)
	void SetOppositeDoor();

	// Synchronize Opposite Door progress
	UFUNCTION(BlueprintCallable)
	void SyncOppositeDoor();
	
	// Checking Door Locked
	FORCEINLINE bool IsLocked() { return bLocked; }

	// Lock Door
	UFUNCTION(BlueprintCallable)
	void Lock();

	// Unlock Door
	UFUNCTION(BlueprintCallable)
	void Unlock();

	// Toggle Lock
	UFUNCTION(BlueprintCallable)
	void ToggleLock();

	// Door Handle Change State
	UFUNCTION()
	void HandleChangeState(bool LeverStatus, EVRInteractibleLeverEventType LeverStatusType, float LeverAngleAtTime);

	// On Gripping Door Handle - Grip Door Too
	UFUNCTION(BlueprintCallable)
	void OnHandleGripDoor();

	// Automatically closing the Door
	UFUNCTION(BlueprintCallable)
	void StartAutoClosing();

	// One Step For Closing
	UFUNCTION(BlueprintCallable)
	void AutoClosingStep();

	// Stop Door Auto Closing Functionality
	UFUNCTION(BlueprintCallable)
	void StopAutoClosing();

	// Init Door Auto closing
	UFUNCTION(BlueprintCallable)
	void InitDoorAutoClosing();

	// Cancel Door Auto closing
	UFUNCTION(BlueprintCallable)
	void CancelDoorAutoClosing();
};
