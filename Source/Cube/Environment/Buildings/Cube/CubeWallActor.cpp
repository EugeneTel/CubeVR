// Fill out your copyright notice in the Description page of Project Settings.


#include "CubeWallActor.h"

// Sets default values
ACubeWallActor::ACubeWallActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Prepare Wall Mesh
	WallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallMesh"));
	RootComponent = WallMesh;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> WallMeshAsset(TEXT("StaticMesh'/Game/Cube/Environment/Buildings/Cube/Meshes/SM_CubeWall.SM_CubeWall'"));
	if (WallMeshAsset.Succeeded()) {
		WallMesh->SetStaticMesh(WallMeshAsset.Object);
	}

	// Prepare Ladder Mesh
	LadderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LadderMesh"));
	LadderMesh->SetupAttachment(GetRootComponent());
	LadderMesh->ComponentTags.Add(TEXT("climable"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> LadderMeshAsset(TEXT("StaticMesh'/Game/Cube/Environment/Buildings/Cube/Meshes/SM_CubeLadder.SM_CubeLadder'"));
	if (LadderMeshAsset.Succeeded()) {
		LadderMesh->SetStaticMesh(LadderMeshAsset.Object);
	}

	// Prepare Tunnel Mesh
	TunnelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tunnel"));
	TunnelMesh->SetupAttachment(GetRootComponent());
	TunnelMesh->ComponentTags.Add(TEXT("climable"));
	TunnelMesh->ComponentTags.Add(TEXT("crouchable"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> TunnelMeshAsset(TEXT("StaticMesh'/Game/Cube/Environment/Buildings/Cube/Meshes/SM_CubeTunnel.SM_CubeTunnel'"));
	if (TunnelMeshAsset.Succeeded()) {
		TunnelMesh->SetStaticMesh(TunnelMeshAsset.Object);
	}

	// Prepare Tunnel Collision
	TunnelCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("TunnelCollision"));
	TunnelCollision->SetupAttachment(TunnelMesh);
	TunnelCollision->SetBoxExtent(FVector(40.f, 40.f, 20.f));
	TunnelCollision->SetRelativeLocation(FVector(0.f, 0.f, -20.f));

	// Prepare Door
	DoorSpline = CreateDefaultSubobject<USplineComponent>(TEXT("DoorSpline"));
	DoorSpline->SetupAttachment(RootComponent);
	DoorSpline->SetLocationAtSplinePoint(1, FVector(14.f, 0.f, 17.f), ESplineCoordinateSpace::Local);
	DoorSpline->AddSplineLocalPoint(FVector(85.f, 0.f, 17.f));
	DoorComponent = CreateDefaultSubobject<UCubeDoorComponent>(TEXT("DoorComponent"));
	DoorComponent->SetupAttachment(RootComponent);
	DoorComponent->SplineComponentToFollow = DoorSpline;
}

// Called when the game starts or when spawned
void ACubeWallActor::BeginPlay()
{
	Super::BeginPlay();

	TunnelCollision->OnComponentBeginOverlap.AddDynamic(this, &ACubeWallActor::OnTunnelCollisionOverlapBegin);
	TunnelCollision->OnComponentEndOverlap.AddDynamic(this, &ACubeWallActor::OnTunnelCollisionOverlapEnd);

	// Try to set Opposite Wall
	ACubeWallActor* OutOppositeWall;
	if (FindOppositeWall(OutOppositeWall))
	{
		SetOppositeWall(OutOppositeWall);
	}
	
}

// Called every frame
void ACubeWallActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACubeWallActor::OnTunnelCollisionOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//ULog::Info("---------------------------OnTunnelCollisionOverlapBegin-------------------------", LO_Both);
	AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);

	if (MainCharacter == nullptr)
		return;

	MainCharacter->EnterTunnel();
	bCharacterInTunnel = true;
	DoorComponent->CancelDoorAutoClosing();

	if (OppositeWall != nullptr && IsValid(OppositeWall->DoorComponent))
	{
		OppositeWall->DoorComponent->CancelDoorAutoClosing();
	}
}

void ACubeWallActor::OnTunnelCollisionOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//ULog::Info("------------------------OnTunnelCollisionOverlapEnd------------------------", LO_Both);
	AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);

	if (MainCharacter == nullptr)
		return;

	bCharacterInTunnel = false;

	if (OppositeWall == nullptr || !OppositeWall->bCharacterInTunnel)
	{
		MainCharacter->ExitTunnel();
		DoorComponent->InitDoorAutoClosing();
	}
}


bool ACubeWallActor::FindOppositeWall(ACubeWallActor*& OutOppositeWall)
{
	if (OppositeWall != nullptr)
		return false;

	ULog::Info("---------FindOppositeWall----------");
	TArray<UPrimitiveComponent*> OverlappingComponents;
	TunnelCollision->GetOverlappingComponents(OverlappingComponents);

	for (UPrimitiveComponent* OverlappingComponent : OverlappingComponents)
	{
		if (OverlappingComponent->GetName() == TunnelCollision->GetName())
		{
			OutOppositeWall = Cast<ACubeWallActor>(OverlappingComponent->GetOwner());
			return true;
		}
	}

	return false;
}

void ACubeWallActor::SetOppositeWall(ACubeWallActor* WallActor)
{
	if (OppositeWall != nullptr)
		return;

	// Set for the current wall actor
	OppositeWall = WallActor;
	DoorComponent->OppositeDoor = WallActor->DoorComponent;
	DoorComponent->HandleComponent->OppositeHandle = WallActor->DoorComponent->HandleComponent;

	// Set for the opposite wall actor
	WallActor->SetOppositeWall(this);

	ULog::Success("---------SetOppositeWall----------");
}

