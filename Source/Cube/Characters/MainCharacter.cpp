// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"

// Sets default values
AMainCharacter::AMainCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UMainCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/**
	 * Hands components
	 */
	HandMeshRight = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HandMeshRight"));
	HandMeshRight->SetupAttachment(RightMotionController);

	HandMeshLeft = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HandMeshLeft"));
	HandMeshLeft->SetupAttachment(LeftMotionController);

	GrabSphereRight = CreateDefaultSubobject<USphereComponent>(TEXT("GrabSphereRight"));
	GrabSphereRight->SetupAttachment(RightMotionController);
	GrabSphereRight->SetSphereRadius(4.f);
	// TODO: Prepare collision presets for Sphere

	GrabSphereLeft = CreateDefaultSubobject<USphereComponent>(TEXT("GrabSphereLeft"));
	GrabSphereLeft->SetupAttachment(LeftMotionController);
	GrabSphereLeft->SetSphereRadius(4.f);
	// TODO: Prepare collision presets for Sphere

	PostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcessComponent"));
	PostProcessComponent->SetupAttachment(GetRootComponent());

	bInTunnel = false;
	bCroaching = false;
	bDead = false;

}

// BeginPlay
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Set Tracking to Floor. Without it in Oculus Rift is under floor.
	UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);

	if (FadeMaterial != nullptr)
	{
		FadeMaterialInstance = UMaterialInstanceDynamic::Create(FadeMaterial, this);
		PostProcessComponent->AddOrUpdateBlendable(FadeMaterialInstance);
		bFading = true;
		FadeProgress = 0.f;
		FadeMaterialInstance->SetScalarParameterValue(TEXT("Radius"), FadeProgress);
		GetWorldTimerManager().SetTimer(FadeTimerHandle, this, &AMainCharacter::FadeOutProgress, 0.01f, true, 0.5f);
	}

}

// SetupPlayerInputComponent
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	// Grip Actions
	PlayerInputComponent->BindAction("GripLeft", IE_Pressed, this, &AMainCharacter::GripLeftPressed);
	PlayerInputComponent->BindAction("GripLeft", IE_Released, this, &AMainCharacter::GripLeftReleased);
	PlayerInputComponent->BindAction("GripRight", IE_Pressed, this, &AMainCharacter::GripRightPressed);
	PlayerInputComponent->BindAction("GripRight", IE_Released, this, &AMainCharacter::GripRightReleased);

	// Movement Actions and Axis
	PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);
	PlayerInputComponent->BindAction("SnapTurnLeft", IE_Pressed, this, &AMainCharacter::SnapTurnLeft);
	PlayerInputComponent->BindAction("SnapTurnRight", IE_Pressed, this, &AMainCharacter::SnapTurnRight);
}

// PostInitializeComponents
void AMainCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	MainCharacterMovementComponent = Cast<UMainCharacterMovementComponent>(Super::GetMovementComponent());
	LevelScript = Cast<ACubeLevelScriptActor>(GetWorld()->GetLevelScriptActor());
}

// OnClimbingSteppedUp
void AMainCharacter::OnClimbingSteppedUp_Implementation()
{
	MainCharacterMovementComponent->StopClimbing();
}

// Tick
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

/*
* Common Logic
*/

// GetNearestOverlappingObject
UPrimitiveComponent* AMainCharacter::GetNearestOverlappingObject(UPrimitiveComponent* OverlapComponent, FName Tag)
{
	if (OverlapComponent == nullptr)
	{
		return nullptr;
	}
	
	// Prepare parameters for ComponentOverlapComponents

	// set collision presets (WorldStatic, WorldDynamic etc.)
	TArray < TEnumAsByte< EObjectTypeQuery >> ComponentTransform;
	ComponentTransform.Add(EObjectTypeQuery::ObjectTypeQuery1);
	ComponentTransform.Add(EObjectTypeQuery::ObjectTypeQuery2);
	ComponentTransform.Add(EObjectTypeQuery::ObjectTypeQuery3);
	ComponentTransform.Add(EObjectTypeQuery::ObjectTypeQuery4);
	ComponentTransform.Add(EObjectTypeQuery::ObjectTypeQuery5);
	ComponentTransform.Add(EObjectTypeQuery::ObjectTypeQuery6);

	// add self to ignore list on collision
	TArray <AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	// prepare variable for results
	TArray <UPrimitiveComponent*> OutComponents;

	// get overlapping components
	if (!UKismetSystemLibrary::ComponentOverlapComponents(OverlapComponent, OverlapComponent->GetComponentTransform(), ComponentTransform, nullptr, ActorsToIgnore, OutComponents))
	{
		return nullptr;
	}

	// Go through all overlapped components and looking for the first valid
	for (UPrimitiveComponent* OutComponent : OutComponents)
	{
		bool bByTag = Tag == "" ? true : OutComponent->ComponentHasTag(Tag);
		if (HasValidGripCollision(OutComponent) && bByTag)
		{
			return OutComponent;
		}
	}

	return nullptr;
}

/**
 * Fade Logic
 */

void AMainCharacter::FadeOutProgress()
{
	if (FadeMaterialInstance == nullptr)
		return;

	FadeProgress += 0.01f;
	FadeMaterialInstance->SetScalarParameterValue(TEXT("Radius"), FadeProgress);

	if (FadeProgress >= 1.f)
	{
		bFading = false;
		GetWorldTimerManager().ClearTimer(FadeTimerHandle);
	}
}

void AMainCharacter::FadeInProgress()
{
	if (FadeMaterialInstance == nullptr)
		return;

	FadeProgress -= 0.003f;
	FadeMaterialInstance->SetScalarParameterValue(TEXT("Radius"), FadeProgress);

	if (FadeProgress <= 0.f)
	{
		bFading = false;
		GetWorldTimerManager().ClearTimer(FadeTimerHandle);
		FadeMaterialInstance->SetScalarParameterValue(TEXT("Radius"), 0.f);
	}
}

/**
* Climbing Logic
*/

// CheckAndHandleClimbing
void AMainCharacter::CheckAndHandleClimbing(UPrimitiveComponent* GrabSphere, UGripMotionControllerComponent* CallingMotionController)
{
	UPrimitiveComponent* OverlappingObject = GetNearestOverlappingObject(GrabSphere, TEXT("climable"));

	if (OverlappingObject == nullptr)
	{
		return;
	}

	if (OverlappingObject->ComponentHasTag(TEXT("crouchable")))
	{
		InitTunnelCroach();
	}

	InitTunnelCroach();
	MainCharacterMovementComponent->InitClimbing(CallingMotionController, OverlappingObject);
}

// CheckAndStopClimbing
void AMainCharacter::CheckAndStopClimbing(UGripMotionControllerComponent* CallingMotionController)
{
	if (MainCharacterMovementComponent->bHandClimbing && CallingMotionController == MainCharacterMovementComponent->ClimbingHand)
	{
		MainCharacterMovementComponent->StopClimbing();
		StopTunnelCroach();
	}
}

/**
* Gripping logic
*/

bool AMainCharacter::CheckAndHandleGripAnimations()
{
	return false;
}

// HasValidGripCollision
bool AMainCharacter::HasValidGripCollision(UPrimitiveComponent* Component)
{
	// Get Collision response for VRTraceChannel
	ECollisionResponse CollisionResponse = Component->GetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1);

	switch (CollisionResponse)
	{
	case ECR_Overlap:
	case ECR_Block:
		return true;
	default:
		return false;
	}
}

/**
* Input Methods
*/
void AMainCharacter::GripLeftPressed()
{
	CheckAndHandleClimbing(GrabSphereLeft, LeftMotionController);
}

void AMainCharacter::GripLeftReleased()
{
	CheckAndStopClimbing(LeftMotionController);
}

void AMainCharacter::GripRightPressed()
{
	CheckAndHandleClimbing(GrabSphereRight, RightMotionController);
}

void AMainCharacter::GripRightReleased()
{
	CheckAndStopClimbing(RightMotionController);
}

void AMainCharacter::SnapTurnLeft()
{
	MainCharacterMovementComponent->PerformMoveAction_SnapTurn(-SnapTurnValue);
}

void AMainCharacter::SnapTurnRight()
{
	MainCharacterMovementComponent->PerformMoveAction_SnapTurn(SnapTurnValue);
}

void AMainCharacter::MoveForward(float Value)
{
	AddMovementInput(GetVRForwardVector(), Value);
}

void AMainCharacter::MoveRight(float Value)
{
	AddMovementInput(GetVRRightVector(), Value);
}

void AMainCharacter::EnterTunnel()
{
	bInTunnel = true;
}

void AMainCharacter::ExitTunnel()
{
	bInTunnel = false;
	StopTunnelCroach();
}

void AMainCharacter::InitTunnelCroach()
{
	if (bCroaching)
		return;

	//ULog::Success("+++++++++++++++++++++++++++++++++InitTunnelCroach++++++++++++++++++++++++++++++++", LO_Both);

	bCroaching = true;

	VRRootReference->SetCapsuleHalfHeightVR(33.f);

	TunnelCroachOffset = VRReplicatedCamera->GetComponentLocation().Z - VRRootReference->GetComponentLocation().Z - 45.f;
	VRRootReference->AddWorldOffset(FVector(0.f, 0.f, TunnelCroachOffset));
	NetSmoother->AddWorldOffset(FVector(0.f, 0.f, -TunnelCroachOffset));
	MainCharacterMovementComponent->MaxWalkSpeed = 80.f;
}

void AMainCharacter::StopTunnelCroach()
{
	if (!bCroaching || bInTunnel || MainCharacterMovementComponent->bHandClimbing)
		return;

	//ULog::Error("+++++++++++++++++++++++++++++++++StopTunnelCroach++++++++++++++++++++++++++++++++", LO_Both);

	bCroaching = false;

	VRRootReference->SetCapsuleHalfHeightVR(96.f);
	VRRootReference->AddWorldOffset(FVector(0.f, 0.f, -TunnelCroachOffset));
	NetSmoother->AddWorldOffset(FVector(0.f, 0.f, TunnelCroachOffset));
	MainCharacterMovementComponent->MaxWalkSpeed = 200.f;
}

void AMainCharacter::Die()
{
	if (bDead)
		return;

	bDead = true;
	MainCharacterMovementComponent->DisableMovement();

	// Set Reload level Timer
	FTimerHandle ReloadTimerHandle;
	GetWorldTimerManager().SetTimer(ReloadTimerHandle, LevelScript, &ACubeLevelScriptActor::ReloadLevel, 3.f, false);

	if (FadeMaterialInstance != nullptr)
	{
		bFading = true;
		FadeProgress = 1.f;
		FadeMaterialInstance->SetScalarParameterValue(TEXT("Radius"), FadeProgress);

		// Set Fade Out Timer
		GetWorldTimerManager().SetTimer(FadeTimerHandle, this, &AMainCharacter::FadeInProgress, 0.01f, true, 1.0f);
	}
}

