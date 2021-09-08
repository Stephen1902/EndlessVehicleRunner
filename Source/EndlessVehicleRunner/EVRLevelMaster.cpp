// Copyright 2021 DME Games

#include "EVRLevelMaster.h"
#include "EVRGameStateBase.h"
#include "EVRSpawnArrowComponent.h"
#include "EVRSpawnMaster.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Player/EVRPlayerVehicle.h"

// Sets default values
AEVRLevelMaster::AEVRLevelMaster()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(SceneComp);

	MeshCompBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Floor Mesh"));
	MeshCompBase->SetupAttachment(RootComponent);
	MeshCompBase->SetRelativeLocation(FVector(500.f, 0.f, 0.f));

	StartCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Start Collision"));
	StartCollision->SetupAttachment(RootComponent);
	StartCollision->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
	StartCollision->OnComponentBeginOverlap.AddDynamic(this, &AEVRLevelMaster::OnBeginOverlapStartComp);
	
	ArrowComp = CreateDefaultSubobject<UArrowComponent>(TEXT("Attach Point"));
	ArrowComp->SetupAttachment(RootComponent);
	ArrowComp->SetRelativeLocation(FVector(1000.f, 0.f, 0.f));

	EndCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("End Collision"));
	EndCollision->SetupAttachment(ArrowComp);
	EndCollision->SetRelativeLocation(FVector(0.f, 0.f,120.f));
	EndCollision->SetBoxExtent(FVector(16.f, 500.f, 100.f));
	
	EndCollision->OnComponentBeginOverlap.AddDynamic(this, &AEVRLevelMaster::OnBeginOverlapEndComp);

	EndCollision->SetHiddenInGame(false);

	SpawnPointLeft = CreateDefaultSubobject<UEVRSpawnArrowComponent>(TEXT("Spawn Point Left"));
	SpawnPointLeft->SetupAttachment(RootComponent);
	SpawnPointLeft->SetRelativeLocation(FVector(100.f, -333.f, 30.f));

	SpawnPointCentre = CreateDefaultSubobject<UEVRSpawnArrowComponent>(TEXT("Spawn Point Centre"));
	SpawnPointCentre->SetupAttachment(RootComponent);
	SpawnPointCentre->SetRelativeLocation(FVector(100.f, 0.f, 30.f));

	SpawnPointRight = CreateDefaultSubobject<UEVRSpawnArrowComponent>(TEXT("Spawn Point Right"));
	SpawnPointRight->SetupAttachment(RootComponent);
	SpawnPointRight->SetRelativeLocation(FVector(100.f, 333.f, 30.f));

	NewSpawnedActor = CreateDefaultSubobject<UChildActorComponent>(TEXT("Spawned Actor"));
	NewSpawnedActor->SetupAttachment(RootComponent);
	
	TurnSpawnChance = 0.f;
	BlockSpawnChance = 50;
}

// Called when the game starts or when spawned
void AEVRLevelMaster::BeginPlay()
{
	Super::BeginPlay();

	GetReferences();
	SetArrowAndBoxTransforms();
	//SpawnPickup();
}

void AEVRLevelMaster::OnBeginOverlapEndComp(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == PlayerVehicleRef)
	{
		if (GameStateRef)
		{
			GameStateRef->TryToSpawnNextPiece();
		}
		
		GetWorld()->GetTimerManager().SetTimer(DestroyTimer, this, &AEVRLevelMaster::DestroyThisPiece, 2.0f, false, -1.f);
	}
}

void AEVRLevelMaster::OnBeginOverlapStartComp(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
//	AEVRSpawnMaster* SpawnedActorHit = Cast<AEVRSpawnMaster>(OtherActor);

	
	
	if ((PlayerVehicleRef && OtherActor == PlayerVehicleRef && !bHasTurnedVehicle) )//|| SpawnedActorHit)
	{
		bHasTurnedVehicle = true;
		
		float CurrentPlayerYaw = PlayerVehicleRef->GetActorRotation().Yaw;

		if (FloorType == EFloorType::FT_Left)
		{
			CurrentPlayerYaw -= 90.f;
		}
		else
		{
			CurrentPlayerYaw += 90.f;
		}

		const FRotator NewPlayerRotation = FRotator(0.f, CurrentPlayerYaw, 0.f);
		PlayerVehicleRef->SetNewPositionAfterTurn(ArrowComp->GetComponentLocation(), NewPlayerRotation);
	}	
}

void AEVRLevelMaster::GetSpawnPointInfo(FVector& NewSpawnLocation, FRotator& NewSpawnRotation) const
{
	NewSpawnLocation = ArrowComp->GetComponentLocation();
	NewSpawnRotation = ArrowComp->GetComponentRotation();
}

void AEVRLevelMaster::SetArrowAndBoxTransforms()
{
	// Only spawn items that can be collided with on the straight pieces
	if (SpawnPointLeft->GetCanSpawnHere())
	{
		SpawnPointArray.Add(SpawnPointLeft->GetRelativeTransform());
	}
	
	if (SpawnPointCentre->GetCanSpawnHere())
	{
		SpawnPointArray.Add(SpawnPointCentre->GetRelativeTransform());
	}

	if (SpawnPointRight->GetCanSpawnHere())
	{
		SpawnPointArray.Add(SpawnPointRight->GetRelativeTransform());
	}

	switch (FloorType)
	{
	case EFloorType::FT_Straight:
	default:
		StartCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);	
		break;
	case EFloorType::FT_Left:
		ArrowComp->SetRelativeLocation(FVector(500.f, -500.f, 0.f));
		ArrowComp->SetRelativeRotation(FRotator(0.f, 270.f, 0.f));
		break;
	case EFloorType::FT_Right:
		ArrowComp->SetRelativeLocation(FVector(500.f, 500.f, 0.f));
		ArrowComp->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
		break;
	}
}

void AEVRLevelMaster::GetReferences()
{
	GameStateRef = Cast<AEVRGameStateBase>(GetWorld()->GetGameState());

	if (!GameStateRef)
	{
		UE_LOG(LogTemp, Warning, TEXT("Game State Ref failed to set in EVRLevelMaster"));
	}
	else
	{
		GameStateRef->SetEVRLevelMasterRef(this);
	}

	PlayerVehicleRef = Cast<AEVRPlayerVehicle>(GetWorld()->GetFirstPlayerController()->GetPawn());

	if (!PlayerVehicleRef)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Vehicle Ref failed to set in EVRLevelMaster"));
	}
}

void AEVRLevelMaster::DestroyThisPiece()
{
	Destroy();
}

bool AEVRLevelMaster::SpawnPickup()
{
	// Check for valid spawn points and valid items in this level piece's array
	if (SpawnPointArray.Num() > 0 && ItemsToSpawn.Num() > 0)
	{
		// Randomly generated the spawned item from the list of those available and a random location
		const int32 ItemToSpawn = FMath::RandRange(0, ItemsToSpawn.Num() - 1);
		const int32 LocationToSpawn = FMath::RandRange(0, SpawnPointArray.Num() - 1);
	
		NewSpawnedActor->SetChildActorClass(ItemsToSpawn[ItemToSpawn]);
		NewSpawnedActor->SetRelativeTransform(SpawnPointArray[LocationToSpawn]);
		NewSpawnedActor->CreateChildActor();
		return true;
	}

	return false;
}


