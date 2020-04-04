// Fill out your copyright notice in the Description page of Project Settings.


#include "CubeDoorComponent.h"


#include "Log.h"

UCubeDoorComponent::UCubeDoorComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MaxSlideDistance = FVector::ZeroVector;
	MinSlideDistance = FVector::ZeroVector;
	CurrentSliderProgress = 0.f;
	SliderBehaviorWhenReleased = EVRInteractibleSliderDropBehavior::RetainMomentum;
	bLocked = true;
	bDenyGripping = true;
	DoorAutoClosingTime = 15.f;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Game/Cube/Environment/Buildings/Cube/Meshes/SM_CubeDoor.SM_CubeDoor'"));
	if (MeshAsset.Succeeded()) {
		SetStaticMesh(MeshAsset.Object);
	}

	HandleComponent = CreateDefaultSubobject<UCubeDoorHandleComponent>(TEXT("HandleComponent"));
	HandleComponent->SetupAttachment(this);
	HandleComponent->OnLeverStateChanged.AddDynamic(this, &UCubeDoorComponent::HandleChangeState);
}

void UCubeDoorComponent::BeginPlay()
{
	Super::BeginPlay();

	WorldTimerManager = &GetOuter()->GetWorld()->GetTimerManager();
}

void UCubeDoorComponent::OnChildGrip_Implementation(UGripMotionControllerComponent* GrippingController, const FBPActorGripInformation& GripInformation)
{
	//ULog::Success("---------------------------------Door Handle Grip--------------------------------", LO_Both);

	// If user grip a Door Handle - grep the Door too
	if (GripInformation.GrippedObject == HandleComponent)
	{
		OnHandleGripDoor();
	}

	CancelDoorAutoClosing();
}

void UCubeDoorComponent::OnChildGripRelease_Implementation(UGripMotionControllerComponent* ReleasingController, const FBPActorGripInformation& GripInformation, bool bWasSocketed)
{
	InitDoorAutoClosing();
}

void UCubeDoorComponent::OnGrip_Implementation(UGripMotionControllerComponent* GrippingController, const FBPActorGripInformation& GripInformation)
{
	//ULog::Success("---------------------------------Door Grip--------------------------------", LO_Both);
	Super::OnGrip_Implementation(GrippingController, GripInformation);

	CancelDoorAutoClosing();
}

void UCubeDoorComponent::OnGripRelease_Implementation(UGripMotionControllerComponent* ReleasingController, const FBPActorGripInformation& GripInformation, bool bWasSocketed)
{
	Super::OnGripRelease_Implementation(ReleasingController, GripInformation, bWasSocketed);

	InitDoorAutoClosing();
}

void UCubeDoorComponent::TickGrip_Implementation(UGripMotionControllerComponent* GrippingController, const FBPActorGripInformation& GripInformation, float DeltaTime)
{
	if (!bLocked)
	{
		Super::TickGrip_Implementation(GrippingController, GripInformation, DeltaTime);

		SyncOppositeDoor();
	}
}

void UCubeDoorComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	SyncOppositeDoor();
}

void UCubeDoorComponent::StartAutoClosing()
{
	WorldTimerManager->SetTimer(StepAutoClosingTimerHandle, this, &UCubeDoorComponent::AutoClosingStep, 0.001f, true);
}

void UCubeDoorComponent::StopAutoClosing()
{
	//ULog::Error("------------------SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS-------------", LO_Both);

	GetOuter()->GetWorld()->GetTimerManager().ClearTimer(StepAutoClosingTimerHandle);
	Lock();
	HandleComponent->ResetProgress();
}

void UCubeDoorComponent::AutoClosingStep()
{
	//ULog::Success("------------------AutoClosingStep-------------", LO_Both);
	float NewProgress = FMath::Clamp(CurrentSliderProgress - 0.001f, 0.f, 1.f);

	SetSliderProgress(NewProgress);
	SyncOppositeDoor();

	if (NewProgress <= 0.f)
	{
		StopAutoClosing();
	}

}

void UCubeDoorComponent::SetOppositeDoor()
{
	if (IsValid(HandleComponent) && OppositeDoor != nullptr && IsValid(OppositeDoor->HandleComponent))
	{
		HandleComponent->OppositeHandle = OppositeDoor->HandleComponent;
	}
}

void UCubeDoorComponent::SyncOppositeDoor()
{
	if (IsValid(OppositeDoor))
	{
		OppositeDoor->SetSliderProgress(CurrentSliderProgress);
	}
}

void UCubeDoorComponent::Lock()
{
	bLocked = true;
	bDenyGripping = true;
}

void UCubeDoorComponent::Unlock()
{
	bLocked = false;
	bDenyGripping = false;
}

void UCubeDoorComponent::ToggleLock()
{
	if (IsLocked())
	{
		Unlock();
	}
	else {
		Lock();
	}
}

void UCubeDoorComponent::OnHandleGripDoor()
{
	TArray<FBPGripPair> HoldingControllers;
	bool bIsHandleHeld;

	HandleComponent->IsHeld(HoldingControllers, bIsHandleHeld);

	if (!bIsHandleHeld)
		return;

	UGripMotionControllerComponent* HoldingController = HoldingControllers[0].HoldingController;

	// check if controller already held the door slider
	if (HoldingController->GetIsComponentHeld(this))
		return;

	HoldingController->GripObjectByInterface(this, GetComponentTransform());
}

void UCubeDoorComponent::InitDoorAutoClosing()
{
	WorldTimerManager->SetTimer(InitAutoClosingTimerHandle, this, &UCubeDoorComponent::StartAutoClosing, DoorAutoClosingTime, false);
}

void UCubeDoorComponent::CancelDoorAutoClosing()
{
	if (WorldTimerManager->IsTimerActive(InitAutoClosingTimerHandle))
	{
		WorldTimerManager->ClearTimer(InitAutoClosingTimerHandle);
	}

}

void UCubeDoorComponent::HandleChangeState(bool LeverStatus, EVRInteractibleLeverEventType LeverStatusType, float LeverAngleAtTime)
{
	//ULog::Success("------------------HANDLE BLOCK-----------------", LO_Both);
	HandleComponent->Lock();
	Unlock();
}