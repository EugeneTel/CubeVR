// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "Log.h"

void UMainCharacterMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

// Tick Comp
void UMainCharacterMovementComponent::TickComponent(
	float DeltaTime,
	enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction
	) 
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

// PhysCustom_Climbing
void UMainCharacterMovementComponent::PhysCustom_Climbing(float deltaTime, int32 Iterations)
{
	if (GrippedObject != nullptr && ClimbingHand != nullptr && bHandClimbing)
	{
		FVector GripVector = UKismetMathLibrary::TransformLocation(GrippedObject->GetComponentTransform(), ClimbGripLocation);
		FVector MovementVector = UKismetMathLibrary::NegateVector(ClimbingHand->GetComponentLocation() - GripVector);

		AddCustomReplicatedMovement(MovementVector);
	}

	Super::PhysCustom_Climbing(deltaTime, Iterations);
}

// InitClimbing
void UMainCharacterMovementComponent::InitClimbing(UGripMotionControllerComponent* Hand, UPrimitiveComponent* Object)
{
	ClimbGripLocation = UKismetMathLibrary::InverseTransformLocation(Object->GetComponentTransform(), Hand->GetComponentLocation());
	GrippedObject = Object;
	ClimbingHand = Hand;
	bHandClimbing = true;
	SetClimbingMode(true);
}

// StopClimbing
void UMainCharacterMovementComponent::StopClimbing()
{
	if (!bHandClimbing)
	{
		return;
	}

	SetClimbingMode(false);
	bHandClimbing = false;
	ClimbGripLocation = FVector::ZeroVector;
	ClimbingHand = nullptr;
	GrippedObject = nullptr;
}
