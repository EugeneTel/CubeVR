// Fill out your copyright notice in the Description page of Project Settings.


#include "CubeDoorSliderComponent.h"
#include "Engine/World.h"

#include "Log.h"

UCubeDoorSliderComponent::UCubeDoorSliderComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MaxSlideDistance = FVector::ZeroVector;
	MinSlideDistance = FVector::ZeroVector;
	CurrentSliderProgress = 0.f;
	SliderBehaviorWhenReleased = EVRInteractibleSliderDropBehavior::RetainMomentum;
	bLocked = false;
	bDenyGripping = false;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Game/Cube/Environment/Buildings/Cube/Meshes/SM_CubeDoor.SM_CubeDoor'"));
	if (MeshAsset.Succeeded()) {
		SetStaticMesh(MeshAsset.Object);
	}
}

void UCubeDoorSliderComponent::TickGrip_Implementation(UGripMotionControllerComponent* GrippingController, const FBPActorGripInformation& GripInformation, float DeltaTime)
{
	if (!bLocked)
	{
		Super::TickGrip_Implementation(GrippingController, GripInformation, DeltaTime);

		SyncOppositeSlider();
	}
}

void UCubeDoorSliderComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	SyncOppositeSlider();
}

void UCubeDoorSliderComponent::StartAutoClosing()
{
	ULog::Error("------------------StartAutoClosing-------------", LO_Both);

	GetOuter()->GetWorld()->GetTimerManager().SetTimer(AutoClosingTimerHandle, this, &UCubeDoorSliderComponent::AutoClosingStep, 0.001f, true);
}

void UCubeDoorSliderComponent::AutoClosingStep()
{
	//ULog::Success("------------------AutoClosingStep-------------", LO_Both);
	float NewProgress = FMath::Clamp(CurrentSliderProgress - 0.001f, 0.f, 1.f);

	SetSliderProgress(NewProgress);
	SyncOppositeSlider();

	if (NewProgress <= 0.f)
	{
		StopAutoClosing();
	}

}

void UCubeDoorSliderComponent::StopAutoClosing()
{
	GetOuter()->GetWorld()->GetTimerManager().ClearTimer(AutoClosingTimerHandle);
}


void UCubeDoorSliderComponent::SyncOppositeSlider()
{
	if (IsValid(OppositeSlider))
	{
		OppositeSlider->SetSliderProgress(CurrentSliderProgress);
	}
}

void UCubeDoorSliderComponent::Lock()
{
	bLocked = true;
	bDenyGripping = true;
}

void UCubeDoorSliderComponent::Unlock()
{
	bLocked = false;
	bDenyGripping = false;
}

void UCubeDoorSliderComponent::ToggleLock()
{
	if (IsLocked())
	{
		Unlock();
	}
	else {
		Lock();
	}
}
