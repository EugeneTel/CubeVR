// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CubeTrapInterface.h"

#include "CubeBaseTrapActor.generated.h"

UCLASS(Abstract, Blueprintable)
class CUBE_API ACubeBaseTrapActor : public AActor, public ICubeTrapInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACubeBaseTrapActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bActive;

	// Check is a Trap Active
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Trap Interface")
	bool IsActive();

	// Interact a Trap
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Trap Interface")
	void Interact();

	// Deactivate a Trap
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Trap Interface")
	void Deactivate();

};
