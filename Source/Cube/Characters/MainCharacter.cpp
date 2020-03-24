// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "Components/SphereComponent.h"

// Sets default values
AMainCharacter::AMainCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GrabSphereRight = CreateDefaultSubobject<USphereComponent>(TEXT("GrabSphereRight"));
	GrabSphereRight->SetupAttachment(RightMotionController);
	GrabSphereRight->SetSphereRadius(4.f);

	GrabSphereLeft = CreateDefaultSubobject<USphereComponent>(TEXT("GrabSphereLeft"));
	GrabSphereLeft->SetupAttachment(LeftMotionController);
	GrabSphereLeft->SetSphereRadius(4.f);
}