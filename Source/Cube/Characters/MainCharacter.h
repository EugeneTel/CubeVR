// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VRCharacter.h"
#include "MainCharacterMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/PostProcessComponent.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "TimerManager.h"
#include "Cube/Levels/CubeLevelScriptActor.h"
#include "Sound/SoundCue.h"
#include "Enums/GripState.h"
#include "VRGripInterface.h"
#include "Animations/HandAnimInstance.h"

#include "Log.h"
#include "Cube/Props/ShoeActor.h"

#include "MainCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGripDropped, UGripMotionControllerComponent*, GripController, UObject*, DroppedObject);

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

	// Called upon landing when falling
	virtual void Landed(const FHitResult& Hit) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	/**
	* Common properties and methods
	*/
	
	UPROPERTY(BlueprintReadOnly)
    class ACubeLevelScriptActor* LevelScript;
	
	UPROPERTY(BlueprintReadOnly)
	class UMainCharacterMovementComponent* MainCharacterMovementComponent;

	UPROPERTY(BlueprintReadOnly)
	class UPawnNoiseEmitterComponent* NoiseEmitter;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UPostProcessComponent* PostProcessComponent;

	UPROPERTY(EditAnywhere)
    UMaterialInterface* FadeMaterial;

	UPROPERTY()
    UMaterialInstanceDynamic* FadeMaterialInstance;

	UPROPERTY()
    float FadeProgress;

	UPROPERTY()
    bool bFading;

	UPROPERTY()
    FTimerHandle FadeTimerHandle;

	UFUNCTION()
    void FadeInProgress();

	UFUNCTION()
    void FadeOutProgress();

	// Get Nearest overlapping object by tag
	UFUNCTION(BlueprintCallable)
	UPrimitiveComponent* GetNearestOverlappingObject(UPrimitiveComponent* OverlapComponent, FName Tag = "");

	// Attach a Shoe to the Body socket
	UFUNCTION(BlueprintCallable)
	void CheckAndAttachShoeToBody(AShoeActor* ShoeActor) const;

	/**
	 * Body properties and methods
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body")
	UStaticMeshComponent* BodyMesh;

	// Set body position
	UFUNCTION(BlueprintCallable)
	void UpdateBodyPosition() const;


	/**
	* Hands properties and methods
	*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hands")
	class USphereComponent* GrabSphereRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hands")
	class USphereComponent* GrabSphereLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hands")
	class USkeletalMeshComponent* HandMeshLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hands")
	class USkeletalMeshComponent* HandMeshRight;
	
	UFUNCTION(BlueprintCallable)
    bool TraceFromController(UGripMotionControllerComponent* CallingController, FHitResult& OutHitResult) const;

	/**
	 * Gripping properties and methods
	 */
	UPROPERTY()
	FOnGripDropped OnGripDropped;
	
	UPROPERTY(BlueprintReadWrite)
	bool bGripPressedLeft;

	UPROPERTY(BlueprintReadWrite)
	bool bGripPressedRight;

	UPROPERTY(BlueprintReadWrite)
	EGripState GripStateLeft;

	UPROPERTY(BlueprintReadWrite)
	EGripState GripStateRight;

	
	UFUNCTION(BlueprintCallable)
    void CheckAndHandleGrip(UPrimitiveComponent* GrabSphere, UGripMotionControllerComponent* CallingController);

	UFUNCTION(BlueprintCallable)
    void DropAllGrips(UGripMotionControllerComponent* CallingController) const;

	UFUNCTION()
	void GripDropped(UGripMotionControllerComponent* GripController, UObject* DroppedObject);

	UFUNCTION(BlueprintCallable)
	void CheckAndHandleGripAnimations();

	// Set Grip Controller Animation property
	UFUNCTION(BlueprintCallable)
	void CheckAndHandleGripControllerAnimations(UPrimitiveComponent* GrabSphere, UGripMotionControllerComponent* CallingController, bool bGripPressed, EGripState& GripState);

	// Check is a Component valid for grip collision
	UFUNCTION(BlueprintCallable)
	static bool HasValidGripCollision(UPrimitiveComponent* Component);

	// Grip Input bindings
	UFUNCTION(BlueprintCallable)
    void GripLeftPressed();

	UFUNCTION(BlueprintCallable)
    void GripRightPressed();

	UFUNCTION(BlueprintCallable)
    void GripLeftReleased();

	UFUNCTION(BlueprintCallable)
    void GripRightReleased();


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

	// Movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SnapTurnValue = 45.f;

	UFUNCTION(BlueprintCallable)
	void SnapTurnLeft();

	UFUNCTION(BlueprintCallable)
	void SnapTurnRight();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bMoveForward;

	UFUNCTION(BlueprintCallable)
	void MoveForward(const float Value);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bMoveRight;

	UFUNCTION(BlueprintCallable)
	void MoveRight(const float Value);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bCrouching;

	// Tunnel crouch
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bInTunnel;

	// Enter to the Tunnel Collision
	UFUNCTION(BlueprintCallable)
	void EnterTunnel();

	// Exit from all Tunnels
	UFUNCTION(BlueprintCallable)
	void ExitTunnel();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float TunnelCrouchOffset;

	UFUNCTION(BlueprintCallable)
	void InitTunnelCrouch();

	UFUNCTION(BlueprintCallable)
	void StopTunnelCrouch();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDead;

	UFUNCTION(BlueprintCallable)
	void Die();

	/**
	 * Sound properties and methods
	 */
protected:

	// position of the previous step
	FVector StepLastPosition;

	// Current distance from previous step
	float StepCurrentDistance;

	// is just starting walking
	bool bStepBegin;

	// is landing sound playing (TODO: refactor it)
	bool bLandingSoundPlaying;

public:
	
	// distance of one step
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	float StepLength;

	// list of footsteps assets
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sound")
	TArray<USoundCue*> FootstepsAssetList;

	// Play footsteps sound
	UFUNCTION(BlueprintCallable, Category = "Sound")
	void CheckAndPlayFootstepsSound();

	// Reset footsteps sound to begin
	UFUNCTION(BlueprintCallable, Category = "Sound")
	void ResetFootstepsSound();

	// Check Is footsteps sound is active
	UFUNCTION(BlueprintCallable, Category = "Sound")
	bool IsFootstepsSoundActive();

	// Play sound on landing
	UFUNCTION(BlueprintCallable, Category = "Sound")
	void CheckAndPlayLandedSound();

	// Check Is landing sound is active
	UFUNCTION(BlueprintCallable, Category = "Sound")
	bool IsLandedSoundActive();
};
