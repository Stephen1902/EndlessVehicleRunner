// Copyright 2021 DME Games

#include "EVRLevelMaster.h"
#include "EVRGameStateBase.h"
#include "EVRSpawnArrowComponent.h"
#include "EVRSpawnMaster.h"
#include "Vehicles/EVRVehiclePlayer.h"
#include "Vehicles/EVRVehicleMaster.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "EVRRoadsideSpawns.h"
#include "Vehicles/EVRVehicleAI.h"

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
	//StartCollision->OnComponentBeginOverlap.AddDynamic(this, &AEVRLevelMaster::OnBeginOverlapStartComp);
	
	ArrowComp = CreateDefaultSubobject<UArrowComponent>(TEXT("Attach Point"));
	ArrowComp->SetupAttachment(RootComponent);
	ArrowComp->SetRelativeLocation(FVector(1000.f, 0.f, 0.f));

	EndCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("End Collision"));
	EndCollision->SetupAttachment(ArrowComp);
	EndCollision->SetRelativeLocation(FVector(0.f, 0.f,120.f));
	EndCollision->SetBoxExtent(FVector(16.f, 500.f, 100.f));
	EndCollision->OnComponentBeginOverlap.AddDynamic(this, &AEVRLevelMaster::OnBeginOverlapEndComp);

	SpawnPointLeft = CreateDefaultSubobject<UEVRSpawnArrowComponent>(TEXT("Spawn Point Left"));
	SpawnPointLeft->SetupAttachment(RootComponent);
	SpawnPointLeft->SetRelativeLocation(FVector(100.f, -333.f, 30.f));

	SpawnPointCentre = CreateDefaultSubobject<UEVRSpawnArrowComponent>(TEXT("Spawn Point Centre"));
	SpawnPointCentre->SetupAttachment(RootComponent);
	SpawnPointCentre->SetRelativeLocation(FVector(100.f, 0.f, 30.f));

	SpawnPointRight = CreateDefaultSubobject<UEVRSpawnArrowComponent>(TEXT("Spawn Point Right"));
	SpawnPointRight->SetupAttachment(RootComponent);
	SpawnPointRight->SetRelativeLocation(FVector(100.f, 333.f, 30.f));

	OnRoadActor = CreateDefaultSubobject<UChildActorComponent>(TEXT("Spawned Pickup Actor"));
	OnRoadActor->SetupAttachment(RootComponent);

	NewRoadsideActor = CreateDefaultSubobject<UChildActorComponent>(TEXT("Spawned Roadside Actor"));
	NewRoadsideActor->SetupAttachment(RootComponent);
	
	TurnSpawnChance = 0.f;
	BlockSpawnChance = 50;
	bLastPieceWasPark = false;
}

// Called when the game starts or when spawned
void AEVRLevelMaster::BeginPlay()
{
	Super::BeginPlay();

	GetReferences();
	SetArrowAndBoxTransforms();
	SpawnRoadsidePiece();
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
		SpawnPointArray.Add(SpawnPointLeft->GetComponentTransform());
	}
	
	if (SpawnPointCentre->GetCanSpawnHere())
	{
		SpawnPointArray.Add(SpawnPointCentre->GetComponentTransform());
	}

	if (SpawnPointRight->GetCanSpawnHere())
	{
		SpawnPointArray.Add(SpawnPointRight->GetComponentTransform());
	}

	switch (FloorType)
	{
	case EFloorType::FT_Straight:
	default:
		StartCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		StartCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
		StartCollision->SetGenerateOverlapEvents(false);
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

	PlayerVehicleRef = Cast<AEVRVehiclePlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());

	if (!PlayerVehicleRef)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Vehicle Ref failed to set in EVRLevelMaster"));
	}
}

void AEVRLevelMaster::SpawnRoadsidePiece()
{
	// Check for valid roadside pieces available for spawn
	if (RoadsidePiecesToSpawn.Num() > 0)
	{
		int32 ItemToSpawn = FMath::RandRange(0, RoadsidePiecesToSpawn.Num() - 1);
		
		if (AEVRRoadsideSpawns* PieceToSpawn = Cast<AEVRRoadsideSpawns>(RoadsidePiecesToSpawn[ItemToSpawn].GetDefaultObject()))
		{
			// Check if last piece spawned was a park piece and if this one is, keep looping until it isn't
			while (bLastPieceWasPark && PieceToSpawn->GetIsParkPiece())
			{
				ItemToSpawn = FMath::RandRange(0, RoadsidePiecesToSpawn.Num() - 1);
				PieceToSpawn = Cast<AEVRRoadsideSpawns>(RoadsidePiecesToSpawn[ItemToSpawn].GetDefaultObject());
			}

			// Check if this new item is a park piece
			if (PieceToSpawn->GetIsParkPiece())
			{
				bLastPieceWasPark = true;
			}
			else
			{
				bLastPieceWasPark = false;
			}
		
			NewRoadsideActor->SetChildActorClass(RoadsidePiecesToSpawn[ItemToSpawn]);
			NewRoadsideActor->SetRelativeLocation(FVector(500.f, 0.f, 0.f));
			//NewRoadsideActor->CreateChildActor();
		}
			
	}
}

void AEVRLevelMaster::DestroyThisPiece()
{
	Destroy();
}


void AEVRLevelMaster::AddVehicleToTurnedVehicleArray(AEVRVehicleMaster* VehicleIn)
{
	TurnedThisVehicle.Add(VehicleIn);
}

bool AEVRLevelMaster::SpawnPickup()
{
	// Check for valid spawn points and valid items in this level piece's array
	if (SpawnPointArray.Num() > 0 && NonPlayerVehicles.Num() > 0)
	{
		// Randomly generated the spawned item from the list of those available and a random location
		const int32 RandomVehicleToSpawn = FMath::RandRange(0, NonPlayerVehicles.Num() - 1);
		const int32 RandomTransformToSpawnAt = FMath::RandRange(0,2);
		
		const FActorSpawnParameters SpawnInfo;
		FVector WorldLocationToSpawn = ArrowComp->GetComponentLocation();
		WorldLocationToSpawn.Z += 20.f;
		
		AEVRVehicleAI* NewVehicle = GetWorld()->SpawnActor<AEVRVehicleAI>(NonPlayerVehicles[RandomVehicleToSpawn], WorldLocationToSpawn, ArrowComp->GetComponentRotation(), SpawnInfo); 

		// Check where the vehicle has spawned, adjusting the bHasTurned variable to ensure it turns correctly
		const float ArrowRelativeYaw = ArrowComp->GetComponentRotation().Yaw;

		// Check if the vehicle is moving along the "Y" axis, set as required for turning
		if (FMath::IsNearlyEqual(ArrowRelativeYaw, 90.f, 1.f) || FMath::IsNearlyEqual(ArrowRelativeYaw, -90.f, 1.f))
		{
			NewVehicle->SetHasTurned(true);

			NewVehicle->AddToLaneLocations(NewVehicle->GetActorLocation().X - GameStateRef->GetDistanceBetweenLanes());
			NewVehicle->AddToLaneLocations(NewVehicle->GetActorLocation().X);
			NewVehicle->AddToLaneLocations(NewVehicle->GetActorLocation().X + GameStateRef->GetDistanceBetweenLanes());
		}
		else
		{
			NewVehicle->SetHasTurned(false);
			
			NewVehicle->AddToLaneLocations(NewVehicle->GetActorLocation().Y - GameStateRef->GetDistanceBetweenLanes());
			NewVehicle->AddToLaneLocations(NewVehicle->GetActorLocation().Y);
			NewVehicle->AddToLaneLocations(NewVehicle->GetActorLocation().Y + GameStateRef->GetDistanceBetweenLanes());
			
		}
		//OnRoadActor->SetChildActorClass(NonPlayerVehicles[ItemToSpawn]);
		
		if (SpawnPointArray.Num() == 3)
		{
			switch (RandomTransformToSpawnAt)
			{
				case 0:
					NewVehicle->TurnLeft();
					break;
				case 1:
				default:
					break;
				case 2:
					NewVehicle->TurnRight();
					break;
			}
		}

		
		//OnRoadActor->CreateChildActor();
		return true;
	}

	return false;
}


