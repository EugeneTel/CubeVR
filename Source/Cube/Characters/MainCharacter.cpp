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

	/**
	 * Body preparations
	 */
	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(NetSmoother);
	BodyMesh->SetCollisionProfileName(TEXT("NoCollision"));

	BodySocketLeft.BodyMesh = BodyMesh;
	BodySocketLeft.Name = TEXT("ShoeLeft");
	BodySocketLeft.bBusy = false;
	
	BodySocketRight.BodyMesh = BodyMesh;
	BodySocketRight.Name = TEXT("ShoeRight");
	BodySocketRight.bBusy = false;

	/**
	 * Common preparation
	 */
	PostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcessComponent"));
	PostProcessComponent->SetupAttachment(GetRootComponent());

	NoiseEmitter = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("NoiseEmitter"));

	bInTunnel = false;
	bCrouching = false;
	bDead = false;
	bStepBegin = true;
	bLandingSoundPlaying = false;

	StepLength = 80.f;

}

// BeginPlay
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Binding Multi-Cast Delegates
	OnGripDropped.AddDynamic(this, &AMainCharacter::GripDropped);

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

void AMainCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	CheckAndPlayLandedSound();
}

// Tick
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckAndPlayFootstepsSound();

	UpdateBodyPosition();

	CheckAndHandleGripAnimations();
}

/*
* Common Logic
*/

// GetNearestOverlappingObject
UPrimitiveComponent* AMainCharacter::GetNearestOverlappingObject(UPrimitiveComponent* OverlapComponent, const FName Tag)
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
		const bool bByTag = Tag == "" ? true : OutComponent->ComponentHasTag(Tag);
		if (HasValidGripCollision(OutComponent) && bByTag)
		{
			return OutComponent;
		}
	}

	return nullptr;
}

void AMainCharacter::CheckAndAttachShoeToBody(AShoeActor* ShoeActor)
{
	const float MinAttachDistance = 15.f;

	// Attach only to not busy sockets
	if (BodySocketLeft.IsBusy() && BodySocketRight.IsBusy())
		return;
	
	float RightSocketDistance;
	const bool RightSocketValid = BodySocketRight.CalculateDistanceToPoint(ShoeActor->GetActorLocation(), RightSocketDistance);
	
	float LeftSocketDistance;
	const bool LeftSocketValid = BodySocketLeft.CalculateDistanceToPoint(ShoeActor->GetActorLocation(), LeftSocketDistance);

	if (!RightSocketValid || !LeftSocketValid)
	{
		UE_LOG(LogTemp, Error, TEXT("Some of Body Sockets is not valid or not found!"));
		return;
	}
	
	// Check is BodySocketLeft suitable for attachment
	if (!BodySocketLeft.IsBusy() && LeftSocketDistance < MinAttachDistance && LeftSocketDistance < RightSocketDistance)
	{
		AttachShoeToBody(ShoeActor, BodySocketLeft);
	} else
	{
		// Check is BodySocketRight suitable for attachment
		if (!BodySocketRight.IsBusy() && RightSocketDistance < MinAttachDistance)
		{
			AttachShoeToBody(ShoeActor, BodySocketRight);
		}
	}
}

void AMainCharacter::AttachShoeToBody(AShoeActor* ShoeActor, FBodySocket& BodySocket) const
{
	// Attach the Shoe to the Suitable Socket
	const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
	ShoeActor->AttachToComponent(BodyMesh, AttachmentRules, BodySocket.Name);
	ShoeActor->GetStaticMeshComponent()->SetCollisionProfileName("OverlapOnlyPawn");
	BodySocket.Attach(ShoeActor);
}

void AMainCharacter::CheckAndDetachShoeFromBody(AShoeActor* ShoeActor)
{
	// Check sockets
	if (BodySocketRight.IsBusy() && BodySocketRight.AttachedActor->GetClass() == ShoeActor->GetClass())
	{
		BodySocketRight.Detach();
		ShoeActor->GetStaticMeshComponent()->SetCollisionProfileName("PhysicsActor");
	} else if (BodySocketLeft.IsBusy() && BodySocketLeft.AttachedActor->GetClass() == ShoeActor->GetClass())
	{
		BodySocketLeft.Detach();
		ShoeActor->GetStaticMeshComponent()->SetCollisionProfileName("PhysicsActor");
	}
}

void AMainCharacter::UpdateBodyPosition() const
{
	FVector BodyLocation = VRReplicatedCamera->GetRelativeLocation();
	BodyLocation -= FVector(0.f, 0.f, 30.f);
	BodyMesh->SetRelativeLocation(BodyLocation);

	FRotator BodyRotator = NetSmoother->GetComponentRotation();
	BodyRotator -= FRotator(0.f, 90.f, 0.f);
	BodyMesh->SetWorldRotation(BodyRotator);
}

bool AMainCharacter::TraceFromController(UGripMotionControllerComponent* CallingController, FHitResult& OutHitResult) const
{
	const float MaxTraceDistance = 500.f;

	const FVector TraceStart = CallingController->GetComponentLocation();
	const FVector TraceEnd = TraceStart + (CallingController->GetForwardVector() * MaxTraceDistance);

	const TArray<AActor*> ActorsToIgnore;
	return UKismetSystemLibrary::SphereTraceSingle(GetWorld(), TraceStart, TraceEnd, 5.f, ETraceTypeQuery::TraceTypeQuery3, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, OutHitResult, true, FLinearColor::Green, FLinearColor::Red, 0.3f);
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
		InitTunnelCrouch();
	}

	InitTunnelCrouch();
	ResetFootstepsSound();
	MainCharacterMovementComponent->InitClimbing(CallingMotionController, OverlappingObject);
}

// CheckAndStopClimbing
void AMainCharacter::CheckAndStopClimbing(UGripMotionControllerComponent* CallingMotionController)
{
	if (MainCharacterMovementComponent->bHandClimbing && CallingMotionController == MainCharacterMovementComponent->ClimbingHand)
	{
		MainCharacterMovementComponent->StopClimbing();
		StopTunnelCrouch();
	}
}

/**
* Gripping logic
*/

void AMainCharacter::CheckAndHandleGripAnimations()
{
	// Left Hand
	CheckAndHandleGripControllerAnimations(GrabSphereLeft, LeftMotionController, bGripPressedLeft, GripStateLeft);
	UHandAnimInstance* LeftAnimInst = Cast<UHandAnimInstance>(HandMeshLeft->GetAnimInstance());
	LeftAnimInst->GripState = GripStateLeft;
	
	// Right Hand
	CheckAndHandleGripControllerAnimations(GrabSphereRight, RightMotionController, bGripPressedRight, GripStateRight);
	UHandAnimInstance* RightAnimInst = Cast<UHandAnimInstance>(HandMeshRight->GetAnimInstance());
	RightAnimInst->GripState = GripStateRight;
}

void AMainCharacter::CheckAndHandleGripControllerAnimations(UPrimitiveComponent* GrabSphere,
	UGripMotionControllerComponent* CallingController, const bool bGripPressed, EGripState& GripState)
{
	if (bGripPressed || CallingController->HasGrippedObjects())
	{
		GripState = EGripState::EGS_Grab;
	} else
	{
		UPrimitiveComponent* NearestObject = GetNearestOverlappingObject(GrabSphere);

		if (IsValid(NearestObject))
		{
			GripState = EGripState::EGS_CanGrab;
		} else
		{
			GripState = EGripState::EGS_Open;
		}
	}
}

// HasValidGripCollision
bool AMainCharacter::HasValidGripCollision(UPrimitiveComponent* Component)
{
	// Get Collision response for VRTraceChannel
	const ECollisionResponse CollisionResponse = Component->GetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1);

	switch (CollisionResponse)
	{
	case ECR_Overlap:
	case ECR_Block:
		return true;
	default:
		return false;
	}
}

void AMainCharacter::CheckAndHandleGrip(UPrimitiveComponent* GrabSphere, UGripMotionControllerComponent* CallingController)
{
	UPrimitiveComponent* NearestObject = GetNearestOverlappingObject(GrabSphere);

	if (!IsValid(NearestObject))
		return;

	if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(NearestObject))
	{
		// Grip Shoe Actor from component
		if (AShoeActor* ShoeActor = Cast<AShoeActor>(PrimComp->GetOwner()))
		{
			ShoeActor->PrepareForGrip();
			CheckAndDetachShoeFromBody(ShoeActor);
			CallingController->GripObjectByInterface(ShoeActor, ShoeActor->GetActorTransform());
		} else // Grip regular component
		{
			if (PrimComp->GetClass()->ImplementsInterface(UVRGripInterface::StaticClass()))
			{
				CallingController->GripObjectByInterface(PrimComp, PrimComp->GetComponentTransform());
			}
		}
	} else
	{
		// Grip Shoe Actor from Actor
		if (AShoeActor* ShoeActor = Cast<AShoeActor>(NearestObject))
		{
			ShoeActor->PrepareForGrip();
			CheckAndDetachShoeFromBody(ShoeActor);
			CallingController->GripObjectByInterface(ShoeActor, ShoeActor->GetActorTransform());
		} else // Grip regular Actor from Actor
		{
			if (AActor* Actor = Cast<AActor>(NearestObject))
			{
				if (Actor->GetClass()->ImplementsInterface(UVRGripInterface::StaticClass()))
				{
					CallingController->GripObjectByInterface(Actor, Actor->GetActorTransform());
				}
			}
		}
	}

}

void AMainCharacter::DropAllGrips(UGripMotionControllerComponent* CallingController) const
{
	if (!CallingController->HasGrippedObjects())
		return;

	TArray<UObject*> GrippedObjectsArray; 

	CallingController->GetGrippedObjects(GrippedObjectsArray);
	
	for (UObject* GrippedObject : GrippedObjectsArray)
	{
		// Drop all objects even if not implements interface
		if (CallingController->DropObjectByInterface(GrippedObject))
		{
			OnGripDropped.Broadcast(CallingController, GrippedObject);
		}
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void AMainCharacter::GripDropped(UGripMotionControllerComponent* GripController, UObject* DroppedObject)
{
	AActor* DroppedActor = Cast<AActor>(DroppedObject);
	if (!DroppedActor)
	{
		if (UPrimitiveComponent * PrimComp = Cast<UPrimitiveComponent>(DroppedObject))
		{
			DroppedActor = PrimComp->GetOwner();
		}
	}

	if (!DroppedActor)
		return;
	
	if (AShoeActor* ShoeActor = Cast<AShoeActor>(DroppedActor))
	{
		CheckAndAttachShoeToBody(ShoeActor);
	}
}

void AMainCharacter::GripLeftPressed()
{
	bGripPressedLeft = true;
	
	CheckAndHandleClimbing(GrabSphereLeft, LeftMotionController);

	CheckAndHandleGrip(GrabSphereLeft, LeftMotionController);
}

void AMainCharacter::GripLeftReleased()
{
	bGripPressedLeft = false;
	
	CheckAndStopClimbing(LeftMotionController);

	DropAllGrips(LeftMotionController);
}

void AMainCharacter::GripRightPressed()
{
	bGripPressedRight = true;
	
	CheckAndHandleClimbing(GrabSphereRight, RightMotionController);

	CheckAndHandleGrip(GrabSphereRight, RightMotionController);
}

void AMainCharacter::GripRightReleased()
{
	bGripPressedRight = false;
	
	CheckAndStopClimbing(RightMotionController);

	DropAllGrips(RightMotionController);
}

/**
* Input Methods
*/

void AMainCharacter::SnapTurnLeft()
{
	MainCharacterMovementComponent->PerformMoveAction_SnapTurn(-SnapTurnValue);
	ResetFootstepsSound();
}

void AMainCharacter::SnapTurnRight()
{
	MainCharacterMovementComponent->PerformMoveAction_SnapTurn(SnapTurnValue);
	ResetFootstepsSound();
}

void AMainCharacter::MoveForward(const float Value)
{
	if (FMath::IsNearlyZero(Value))
	{
		bMoveForward = false;
	}
	else {
		bMoveForward = true;
		AddMovementInput(GetVRForwardVector(), Value);
	}
}

void AMainCharacter::MoveRight(const float Value)
{
	if (FMath::IsNearlyZero(Value))
	{
		bMoveRight = false;
	}
	else {
		bMoveForward = true;
		AddMovementInput(GetVRRightVector(), Value);
	}
}

void AMainCharacter::EnterTunnel()
{
	bInTunnel = true;
}

void AMainCharacter::ExitTunnel()
{
	bInTunnel = false;
	StopTunnelCrouch();
}

void AMainCharacter::InitTunnelCrouch()
{
	if (bCrouching)
		return;

	ResetFootstepsSound();

	bCrouching = true;

	VRRootReference->SetCapsuleHalfHeightVR(35.f);

	TunnelCrouchOffset = VRReplicatedCamera->GetComponentLocation().Z - VRRootReference->GetComponentLocation().Z - 60.f;
	VRRootReference->AddWorldOffset(FVector(0.f, 0.f, TunnelCrouchOffset));
	NetSmoother->AddWorldOffset(FVector(0.f, 0.f, -TunnelCrouchOffset));
	MainCharacterMovementComponent->MaxWalkSpeed = 40.f;
}

void AMainCharacter::StopTunnelCrouch()
{
	if (!bCrouching || bInTunnel || MainCharacterMovementComponent->bHandClimbing)
		return;

	bCrouching = false;

	VRRootReference->SetCapsuleHalfHeightVR(96.f);
	VRRootReference->AddWorldOffset(FVector(0.f, 0.f, -TunnelCrouchOffset));
	NetSmoother->AddWorldOffset(FVector(0.f, 0.f, TunnelCrouchOffset));
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

/**
 * Sound logic
 */

void AMainCharacter::CheckAndPlayFootstepsSound()
{
	if (!IsFootstepsSoundActive() || FootstepsAssetList.Num() == 0 || !StepLength)
		return;
	
	// if just start walking - remember position only
	if (bStepBegin)
	{
		bStepBegin = false;
		StepLastPosition = GetActorLocation();
		return;
	}

	StepCurrentDistance += (GetActorLocation() - StepLastPosition).Size();
	StepLastPosition = GetActorLocation();

	// continue only if made full step
	if (StepCurrentDistance < StepLength)
		return;

	StepCurrentDistance -= StepLength;

	// find and play random sound
	const int32 SoundIndex = FMath::RandRange(0, FootstepsAssetList.Num() - 1);
	UGameplayStatics::PlaySound2D(GetWorld(), FootstepsAssetList[SoundIndex], 0.5f);

	MakeNoise(0.5f, this, GetActorLocation());
}

void AMainCharacter::ResetFootstepsSound()
{
	bStepBegin = true;
}

bool AMainCharacter::IsFootstepsSoundActive() const
{
	return (bMoveForward || bMoveRight) && !bInTunnel && !bCrouching && !bDead && !MainCharacterMovementComponent->bHandClimbing && !MainCharacterMovementComponent->IsFalling();
}

void AMainCharacter::CheckAndPlayLandedSound()
{
	if (FootstepsAssetList.Num() == 0 || !IsLandedSoundActive())
		return;

	// find and play random sound
	int32 SoundIndex = FMath::RandRange(0, FootstepsAssetList.Num() - 1);
	UGameplayStatics::PlaySound2D(GetWorld(), FootstepsAssetList[SoundIndex], 1.f);

	if (!bLandingSoundPlaying)
	{
		bLandingSoundPlaying = true;
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, this, &AMainCharacter::CheckAndPlayLandedSound, 0.05f, false);
	}
	else {
		bLandingSoundPlaying = false;
		MakeNoise(1.f, this, GetActorLocation());
	}
}

bool AMainCharacter::IsLandedSoundActive() const
{
	return !bInTunnel;
}

