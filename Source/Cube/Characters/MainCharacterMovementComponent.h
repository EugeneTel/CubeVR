// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VRCharacterMovementComponent.h"
#include "MainCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class CUBE_API UMainCharacterMovementComponent : public UVRCharacterMovementComponent
{
	GENERATED_BODY()

protected:

	//Init
	virtual void InitializeComponent() override;

	//Tick
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	virtual void PhysCustom_Climbing(float deltaTime, int32 Iterations) override;

	/**
	* Climbing properties and methods
	*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climbing")
	FVector ClimbGripLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climbing")
	UGripMotionControllerComponent* ClimbingHand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climbing")
	bool bHandClimbing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climbing")
	UPrimitiveComponent* GrippedObject;

	UFUNCTION(BlueprintCallable)
	void InitClimbing(UGripMotionControllerComponent* Hand, UPrimitiveComponent* Object);

	UFUNCTION(BlueprintCallable)
	void StopClimbing();
	
};
