// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VRCharacter.h"
#include "MainCharacter.generated.h"

UENUM(BlueprintType)
enum class EGripHand : uint8
{
	EGH_Left UMETA(DisplayName = "Left"),
	EGH_Right UMETA(DisplayName = "Right")
};

/**
 * 
 */
UCLASS()
class CUBE_API AMainCharacter : public AVRCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter(const class FObjectInitializer& ObjectInitializer);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Cache components
	virtual void PostInitializeComponents() override;

	// Called when the client is in climbing mode and is stepped up onto a platform
	virtual void OnClimbingSteppedUp_Implementation() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	class UMainCharacterMovementComponent* MainCharacterMovementComponent;

public:


	/**
	* Common properties and methods
	*/
	UFUNCTION(BlueprintCallable)
	UPrimitiveComponent* GetNearestOverlappingObject(UPrimitiveComponent* OverlapComponent);

	/**
	* Gripping properties and methods
	*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hands")
	class USphereComponent* GrabSphereRight;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hands")
	class USphereComponent* GrabSphereLeft;

	UFUNCTION(BlueprintCallable)
	bool HasValidGripCollision(UPrimitiveComponent* Component);


	/**
	* Climbing properties and methods
	*/
	UFUNCTION(BlueprintCallable)
	void CheckAndHandleClimbing(UPrimitiveComponent* GrabSphere, UGripMotionControllerComponent* CallingMotionController);

	UFUNCTION(BlueprintCallable)
	void CheckAndStopClimbing(UGripMotionControllerComponent* CallingMotionController);

	/**
	* Input properties and methods
	*/

	// Gripping
	UFUNCTION(BlueprintCallable)
	void GripLeftPressed();

	UFUNCTION(BlueprintCallable)
	void GripRightPressed();

	UFUNCTION(BlueprintCallable)
	void GripLeftReleased();

	UFUNCTION(BlueprintCallable)
	void GripRightReleased();

	// Movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SnapTurnValue = 45.f;

	UFUNCTION(BlueprintCallable)
	void SnapTurnLeft();

	UFUNCTION(BlueprintCallable)
	void SnapTurnRight();

	UFUNCTION(BlueprintCallable)
	void MoveForward(float Value);

	UFUNCTION(BlueprintCallable)
	void MoveRight(float Value);

};
