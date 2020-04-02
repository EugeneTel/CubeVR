// Fill out your copyright notice in the Description page of Project Settings.


#include "CubeDoorActor.h"



// Sets default values
ACubeDoorActor::ACubeDoorActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	Spline->SetupAttachment(RootComponent);

	Slider = CreateDefaultSubobject<UCubeDoorSliderComponent>(TEXT("Slider"));
	Slider->SetupAttachment(RootComponent);
	Slider->SplineComponentToFollow = Spline;

	Handle = CreateDefaultSubobject<UCubeDoorHandleComponent>(TEXT("Handle"));
	Handle->SetupAttachment(Slider);
}

void ACubeDoorActor::OnChildGrip_Implementation(UGripMotionControllerComponent* GrippingController, const FBPActorGripInformation& GripInformation)
{
	ULog::Success("----------------Child Grip-------------", LO_Both);

	// If user grip Handle - grep door slider too
	if (GripInformation.GrippedObject == Handle)
	{
		OnHandleGripDoor();
	}

	CancelDoorAutoClosing();
}

void ACubeDoorActor::OnChildGripRelease_Implementation(UGripMotionControllerComponent* ReleasingController, const FBPActorGripInformation& GripInformation, bool bWasSocketed)
{
	InitDoorAutoClosing();
}

// Called when the game starts or when spawned
void ACubeDoorActor::BeginPlay()
{
	Super::BeginPlay();

	Handle->OnLeverStateChanged.AddDynamic(this, &ACubeDoorActor::HandleChangeState);

	if (OppositeDoor != nullptr)
	{
		SetOppositeDoor();
	}
	
}

// Called every frame
void ACubeDoorActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACubeDoorActor::SetOppositeDoor()
{
	if (!IsValid(Slider)) {
		ULog::Error("------------------Slider is not Initialized!!!-----------------", LO_Both);
	}


	if (IsValid(Slider) && OppositeDoor != nullptr && IsValid(OppositeDoor->Slider))
	{ 
		Slider->OppositeSlider = OppositeDoor->Slider;
	}

	if (IsValid(Handle) && OppositeDoor != nullptr && IsValid(OppositeDoor->Handle))
	{
		Handle->OppositeHandle = OppositeDoor->Handle;
	}
}

void ACubeDoorActor::HandleChangeState(bool LeverStatus, EVRInteractibleLeverEventType LeverStatusType, float LeverAngleAtTime)
{
	ULog::Success("------------------HANDLE BLOCK-----------------", LO_Both);
	Handle->Lock();
	Slider->Unlock();
}

void ACubeDoorActor::OnHandleGripDoor()
{
	TArray<FBPGripPair> HoldingControllers;
	bool bIsHeld;

	Handle->IsHeld(HoldingControllers, bIsHeld);

	if (!bIsHeld)
		return;

	UGripMotionControllerComponent* HoldingController = HoldingControllers[0].HoldingController;

	// check if controller already held the door slider
	if (HoldingController->GetIsComponentHeld(Slider))
		return;

	HoldingController->GripObjectByInterface(Slider, Slider->GetComponentTransform());
}

void ACubeDoorActor::InitDoorAutoClosing()
{
	GetWorldTimerManager().SetTimer(DoorAutoClosingTimerHandle, Slider, &UCubeDoorSliderComponent::StartAutoClosing, 5.f, false);
}

void ACubeDoorActor::CancelDoorAutoClosing()
{
	GetWorldTimerManager().ClearTimer(DoorAutoClosingTimerHandle);
}

