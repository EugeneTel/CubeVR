// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Grippables/GrippableStaticMeshComponent.h"
#include "CubeDoorComponent.h"
#include "CubeLadderComponent.h"
#include "Components/BoxComponent.h"
#include "Cube/Characters/MainCharacter.h"

#include "CubeWallActor.generated.h"

UCLASS()
class CUBE_API ACubeWallActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACubeWallActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Opposite Wall Actor
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Cube")
	ACubeWallActor* OppositeWall;

	// Wall Static Mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cube")
	UStaticMeshComponent* WallMesh;

	// Ladder Static Mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cube")
	UStaticMeshComponent* LadderMesh;

	// Tunnel Static Mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cube")
	UStaticMeshComponent* TunnelMesh;

	// Tunnel Collision
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cube")
	UBoxComponent* TunnelCollision;

	UFUNCTION()
	virtual void OnTunnelCollisionOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnTunnelCollisionOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// Door movement route
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cube")
	USplineComponent* DoorSpline;

	// Door Component with Handle
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cube")
	UCubeDoorComponent* DoorComponent;

	// Is Character In the current Wall Tunnel
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Cube")
	bool bCharacterInTunnel;

};
