// Fill out your copyright notice in the Description page of Project Settings.


#include "CubeDoorHandleComponent.h"

UCubeDoorHandleComponent::UCubeDoorHandleComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	LeverRotationAxis = EVRInteractibleLeverAxis::Axis_Z;
	LeverLimitPositive = 178.f;
	LeverLimitNegative = 178.f;
	LeverTogglePercentage = 0.98f;
	GripPriority = 1;
	LeverReturnTypeWhenReleased = EVRInteractibleLeverReturnType::Stay;
	SetCollisionProfileName(TEXT("BlockAllDynamic"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Game/Cube/Environment/Buildings/Cube/Meshes/SM_CubeDoorHandle.SM_CubeDoorHandle'"));
	if (MeshAsset.Succeeded()) {
		SetStaticMesh(MeshAsset.Object);
	}
}

void UCubeDoorHandleComponent::TickGrip_Implementation(UGripMotionControllerComponent* GrippingController, const FBPActorGripInformation& GripInformation, float DeltaTime)
{
	if (!bLocked)
	{
		Super::TickGrip_Implementation(GrippingController, GripInformation, DeltaTime);

		if (IsValid(OppositeHandle))
		{
			//OppositeHandle->AllCurrentLeverAngles = AllCurrentLeverAngles;
			OppositeHandle->SetRelativeRotation(GetRelativeRotation());
		}
	}
}

void UCubeDoorHandleComponent::Lock()
{
	bLocked = true;
	SetLeverAngle(179.98f, FVector::ZeroVector);
	if (IsValid(OppositeHandle))
	{
		OppositeHandle->bLocked = true;
		OppositeHandle->SetLeverAngle(179.98f, FVector::ZeroVector);
	}
}

void UCubeDoorHandleComponent::Unlock()
{
	bLocked = false;

	if (IsValid(OppositeHandle))
	{
		OppositeHandle->bLocked = false;
	}
}

void UCubeDoorHandleComponent::ToggleLock()
{
	if (IsLocked())
	{
		Unlock();
	}
	else {
		Lock();
	}
}

void UCubeDoorHandleComponent::ResetProgress()
{
	Unlock();
	SetLeverAngle(0.f, FVector::ZeroVector);
	if (IsValid(OppositeHandle))
	{
		OppositeHandle->SetLeverAngle(0.f, FVector::ZeroVector);
	}

}


