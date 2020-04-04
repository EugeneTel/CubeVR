// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Grippables/GrippableActor.h"
#include "CubeDoorComponent.h"
#include "CubeDoorHandleComponent.h"
#include "Components/SplineComponent.h"
#include "TimerManager.h"

#include "Log.h"
#include "CubeDoorActor.generated.h"

UCLASS()
class CUBE_API ACubeDoorActor : public AGrippableActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACubeDoorActor(const FObjectInitializer& ObjectInitializer);

	virtual void OnChildGrip_Implementation(UGripMotionControllerComponent* GrippingController, const FBPActorGripInformation& GripInformation) override;
	virtual void OnChildGripRelease_Implementation(UGripMotionControllerComponent* ReleasingController, const FBPActorGripInformation& GripInformation, bool bWasSocketed) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	FTimerHandle DoorAutoClosingTimerHandle;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// Door Actor from the opposite side of wall
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cube")
	ACubeDoorActor* OppositeDoor;

	UFUNCTION(BlueprintCallable)
	void SetOppositeDoor();

	// Door slider component with static mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cube")
	UCubeDoorComponent* Slider;

	// Door handle component with static mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cube")
	UCubeDoorHandleComponent* Handle;

	// Door movement route
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cube")
	USplineComponent* Spline;

	// Door Handle Change State
	UFUNCTION()
	void HandleChangeState(bool LeverStatus, EVRInteractibleLeverEventType LeverStatusType, float LeverAngleAtTime);

	// On Gripping Door Handle - Grip Door Slider Too
	UFUNCTION(BlueprintCallable)
	void OnHandleGripDoor();

	// Init Door Auto closing
	UFUNCTION(BlueprintCallable)
	void InitDoorAutoClosing();

	// Cancel Door Auto closing
	UFUNCTION(BlueprintCallable)
	void CancelDoorAutoClosing();


};
