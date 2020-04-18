// Fill out your copyright notice in the Description page of Project Settings.


#include "CubeSpikesTrapActor.h"

ACubeSpikesTrapActor::ACubeSpikesTrapActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Progress = 0.f;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	// Prepare collider for the Trap
	Collider = CreateDefaultSubobject<UBoxComponent>(TEXT("Collider"));
	Collider->SetupAttachment(RootComponent);
	Collider->SetBoxExtent(FVector(165.f));
	Collider->SetRelativeLocation(FVector(0.f, 0.f, 205.f));

	// Root Component for Instanced Static Meshes
	InstMeshRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("InstMeshRootComponent"));
	InstMeshRootComponent->SetRelativeLocation(FVector::ZeroVector);
	InstMeshRootComponent->SetupAttachment(RootComponent);

	// Instance Static Mesh for each Wall
	SpawnInstMeshList.Add(FTransform(FRotator::ZeroRotator, FVector::ZeroVector));
	SpawnInstMeshList.Add(FTransform(FRotator(-90.0f, 174.28f, -264.28f), FVector(0.f, 200.f, 200.f)));
	SpawnInstMeshList.Add(FTransform(FRotator(-90.0f, -14.03f, -255.96f), FVector(0.f, -200.f, 200.f)));
	SpawnInstMeshList.Add(FTransform(FRotator(-90.0f, 0.f, 0.f), FVector(-200.f, 0.f, 200.f)));
	SpawnInstMeshList.Add(FTransform(FRotator(-90.0f, -6.72f, -173.27f), FVector(200.f, 0.f, 200.f)));
	SpawnInstMeshList.Add(FTransform(FRotator(0.f, 0.f, -180.f), FVector(0.f, 0.f, 400.f)));

	// Spike Instances for each for each Glass
	InstTransformList.Add(FTransform(FVector(-133.f, -133.f, 0.f)));
	InstTransformList.Add(FTransform(FVector(133.f, -133.f, 0.f)));
	InstTransformList.Add(FTransform(FVector(0.f, -133.f, 0.f)));
	InstTransformList.Add(FTransform(FVector(-131.f, 0.f, 0.f)));
	InstTransformList.Add(FTransform(FVector(131.f, 0.f, 0.f)));
	InstTransformList.Add(FTransform(FVector(129.f, 129.f, 0.f)));
	InstTransformList.Add(FTransform(FVector(0.f, 129.f, 0.f)));
	InstTransformList.Add(FTransform(FVector(129.f, 129.f, 0.f)));

	SpawnSpikes();
}

void ACubeSpikesTrapActor::BeginPlay()
{
	Super::BeginPlay();

	InstMeshRootComponent->SetHiddenInGame(true, true);

	for (auto InstMesh : InstMeshList)
	{
		InstMesh->SetRelativeScale3D(FVector(1.f, 1.f, 0.f));
	}

	Collider->OnComponentBeginOverlap.AddDynamic(this, &ACubeSpikesTrapActor::OnColliderOverlapBegin);
}

void ACubeSpikesTrapActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACubeSpikesTrapActor::Interact_Implementation()
{
	InstMeshRootComponent->SetHiddenInGame(false, true);

	// Start Spikes Activation Progress
	GetWorldTimerManager().SetTimer(ProgressTimerHandle, this, &ACubeSpikesTrapActor::ActivationProgress, 0.01f, true, 1.f);
}

void ACubeSpikesTrapActor::Deactivate_Implementation()
{
	// Start Spikes Activation Progress
	GetWorldTimerManager().SetTimer(ProgressTimerHandle, this, &ACubeSpikesTrapActor::DeactivationProgress, 0.01f, true, 3.f);
}

void ACubeSpikesTrapActor::OnColliderOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherComp->ComponentHasTag(TEXT("Trappable")))
	{
		//Interact_Implementation();
	}
}

void ACubeSpikesTrapActor::OnSpikesOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);

	if (MainCharacter != nullptr)
	{
		//FTimerHandle DeathTimerHandle;
		//GetWorldTimerManager().SetTimer(DeathTimerHandle, MainCharacter, &AMainCharacter::Die, 1.f, false, 1.f);
	}
}

void ACubeSpikesTrapActor::SpawnSpikes()
{
	//for (FTransform MeshTransform : SpawnInstMeshList)
	FString MeshName;
	for (int32 Index = 0; Index != SpawnInstMeshList.Num(); ++Index)
	{
		MeshName = TEXT("InstMesh_");
		MeshName.Append(FString::FromInt(Index));
		SpawnInstMesh(*MeshName, SpawnInstMeshList[Index]);
	}
}

void ACubeSpikesTrapActor::SpawnInstMesh(FName MeshName, FTransform MeshTransform)
{
	UInstancedStaticMeshComponent* NewInstMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(MeshName);
	NewInstMesh->SetupAttachment(InstMeshRootComponent);
	NewInstMesh->SetRelativeTransform(MeshTransform);
	//NewInstMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	NewInstMesh->SetCollisionResponseToAllChannels(ECR_Overlap);
	NewInstMesh->SetCollisionProfileName(TEXT("OverlapAll"));
	//NewInstMesh->SetRelativeScale3D(FVector(1.f, 1.f, 0.f));

	NewInstMesh->OnComponentBeginOverlap.AddDynamic(this, &ACubeSpikesTrapActor::OnSpikesOverlapBegin);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Game/Cube/Environment/Traps/Cube/Meshes/SM_TrapSpikes.SM_TrapSpikes'"));
	if (MeshAsset.Succeeded()) {
		NewInstMesh->SetStaticMesh(MeshAsset.Object);
	}

	// Add Static Mesh Instances
	for (FTransform InstTransform : InstTransformList)
	{
		NewInstMesh->AddInstance(InstTransform);
	}

	InstMeshList.Add(NewInstMesh);
}

void ACubeSpikesTrapActor::ActivationProgress()
{
	Progress += 0.02;

	for (auto InstMesh : InstMeshList)
	{
		InstMesh->SetRelativeScale3D(FVector(1.f, 1.f, Progress));
	}

	if (Progress >= 1.f)
	{
		GetWorldTimerManager().ClearTimer(ProgressTimerHandle);
		Deactivate_Implementation();
	}
}

void ACubeSpikesTrapActor::DeactivationProgress()
{
	Progress -= 0.02;

	if (Progress < 0.01f)
	{
		Progress = 0.f;
		InstMeshRootComponent->SetHiddenInGame(true, true);
		GetWorldTimerManager().ClearTimer(ProgressTimerHandle);
	}

	for (auto InstMesh : InstMeshList)
	{
		InstMesh->SetRelativeScale3D(FVector(1.f, 1.f, Progress));
	}
}

void ACubeSpikesTrapActor::CheckCharacterHit()
{
	AMainCharacter* MainCharacter = Cast<AMainCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	if (MainCharacter == nullptr)
		return;

	TArray<AActor*> OverlappingActors;

	GetOverlappingActors(OverlappingActors, ACubeSpikesTrapActor::StaticClass());

	if (OverlappingActors.Num() > 0)
	{
		MainCharacter->Die();
	}
}

