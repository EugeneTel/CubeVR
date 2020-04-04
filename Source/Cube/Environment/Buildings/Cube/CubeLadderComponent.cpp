// Fill out your copyright notice in the Description page of Project Settings.


#include "CubeLadderComponent.h"

UCubeLadderComponent::UCubeLadderComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Game/Cube/Environment/Buildings/Cube/Meshes/SM_CubeLadder.SM_CubeLadder'"));
	if (MeshAsset.Succeeded()) {
		SetStaticMesh(MeshAsset.Object);
	}
}