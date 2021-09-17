// Copyright 2021 DME Games


#include "EVRVehicleMaster.h"

#include "../Player/EVRMovementComponent.h"
#include "Components/BoxComponent.h"
#include "EndlessVehicleRunner/EVRLevelMaster.h"

// Sets default values
AEVRVehicleMaster::AEVRVehicleMaster()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(SceneComponent);
	
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Comp"));
	StaticMeshComp->SetMobility(EComponentMobility::Movable);
	StaticMeshComp->SetupAttachment(SceneComponent);
	StaticMeshComp->OnComponentBeginOverlap.AddDynamic(this, &AEVRVehicleMaster::OnMeshBeginOverlap);

	MovementComponent = CreateDefaultSubobject<UEVRMovementComponent>(TEXT("Movement Component"));

	TurnTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Timeline"));
	
	static ConstructorHelpers::FObjectFinder<UCurveFloat> CurveFloatObject(TEXT("CurveFloat'/Game/Blueprints/Player/FloatCurveNormal.FloatCurveNormal'"));
	FCurve = CurveFloatObject.Object;

	InterpFunction.BindUFunction(this, FName("TimelineFloatReturn"));

	bGameIsPaused = false;
	bIsTurning = false;
	bCanTurn = false;
	MovementSpeed = 500.f;
	SpeedOfAcceleration = 5.0f;
	DistanceBetweenLanes = 200.f;
	TurnSpeed = 0.25f;
	TurnDegrees = 100.f;
	DesiredRotation = 180.f;
	
	TimeSinceTurnStarted = 0.f;
	LocationIndex = 1;

}

void AEVRVehicleMaster::SetNewPositionAfterTurn(const FVector ArrowLocation, const FRotator NewVehicleRotation)
{
	StartVehicleLocation = GetActorLocation();
	StartVehicleRotation = GetActorRotation();
	EndVehicleRotation = NewVehicleRotation;

	if (!bHasTurned)
	{
		EndVehicleLocation = FVector(ArrowLocation.X, StartVehicleLocation.Y, StartVehicleLocation.Z);

		bHasTurned = true;
	}
	else
	{
		EndVehicleLocation = FVector(StartVehicleLocation.X, ArrowLocation.Y, StartVehicleLocation.Z);

		bHasTurned = false;
	}

	if (FCurve)
	{
		TurnTimeline->PlayFromStart();
	}
}

// Called when the game starts or when spawned
void AEVRVehicleMaster::BeginPlay()
{
	Super::BeginPlay();

	GetMovementCurve();
	
	CurrentSpeed = 0.f;
	
}

void AEVRVehicleMaster::TimelineFloatReturn(float Value)
{
	CurrentSpeed = MovementSpeed * 0.75f;
	
	SetActorLocation(FMath::Lerp(StartVehicleLocation, EndVehicleLocation, Value));
	SetActorRotation(FMath::Lerp(StartVehicleRotation, EndVehicleRotation, Value));
}

void AEVRVehicleMaster::OnMeshBeginOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Check if the other actor hit was a level piece
	AEVRLevelMaster* LevelPieceHit = Cast<AEVRLevelMaster>(OtherActor);
	
	if (LevelPieceHit)
	{
		// Check if this vehicle has already been turned by this piece and  that the piece hit was not a straight
		if (!LevelPieceHit->TurnedThisVehicle.Contains(this) && LevelPieceHit->FloorType != EFloorType::FT_Straight)
		{
			LevelPieceHit->AddVehicleToTurnedVehicleArray(this);
			
			// Check that the part which was hit was the Start Collision Box Component
			if (const UBoxComponent* BoxThatWasHit = LevelPieceHit->StartCollision)
			{
				float CurrentVehicleYaw = GetActorRotation().Yaw;

				// Check whether the Yaw needs to be reduced or increase
				if (LevelPieceHit->FloorType == EFloorType::FT_Left)
				{
					CurrentVehicleYaw -= 90.f;
				}
				else
				{
					CurrentVehicleYaw += 90.f;
				}

				const FRotator NewVehicleRotation = FRotator(0.f, CurrentVehicleYaw, 0.f);

				// Get the arrow component of the current piece
				const FVector ArrowCompLocation = LevelPieceHit->ArrowComp->GetComponentLocation();

				SetNewPositionAfterTurn(ArrowCompLocation, NewVehicleRotation);
			}
		}
	}
}

void AEVRVehicleMaster::AddToLaneLocations(const float LocationIn)
{
	YLocations.Add(LocationIn);
}

// Called every frame
void AEVRVehicleMaster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bGameIsPaused)
	{
		MoveVehicleForward(DeltaTime);
		
		if (bIsTurning)
		{
			Turn(DeltaTime);
		}
	}
}

// Called to bind functionality to input
void AEVRVehicleMaster::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEVRVehicleMaster::MoveVehicleForward(float DeltaTime)
{
	if (CurrentSpeed < MovementSpeed)
	{
		CurrentSpeed += MovementSpeed / (MovementSpeed / SpeedOfAcceleration);
	}

	const FVector MoveDirection = FVector(CurrentSpeed * DeltaTime, 0.f, 0.f);
	AddActorLocalOffset(MoveDirection, true);
}

void AEVRVehicleMaster::TurnLeft()
{
	if (!bCanTurn && LocationIndex > 0 && !bIsTurning)
	{
		LastLocationIndex = LocationIndex;
		LocationIndex -= 1;
		bIsTurning = true;
	}
}

void AEVRVehicleMaster::TurnRight()
{
	if (!bCanTurn && LocationIndex < 2 && !bIsTurning)
	{
		LastLocationIndex = LocationIndex;
		LocationIndex += 1;
		bIsTurning = true;
	}
}

void AEVRVehicleMaster::Turn(const float DeltaTime)
{
	FVector NewLocation = StaticMeshComp->GetRelativeLocation();
	FRotator NewRotation = StaticMeshComp->GetRelativeRotation();
	if (LastLocationIndex > LocationIndex)
	{
		NewLocation.Y = StaticMeshComp->GetRelativeLocation().Y - (DeltaTime * (DistanceBetweenLanes / TurnSpeed));
		if (TimeSinceTurnStarted <= TurnSpeed / 2)
		{
			NewRotation.Yaw = StaticMeshComp->GetRelativeRotation().Yaw - (DeltaTime * TurnDegrees);
		}
		else
		{
			NewRotation.Yaw = StaticMeshComp->GetRelativeRotation().Yaw + (DeltaTime * TurnDegrees);
		}
	}
	else
	{
		NewLocation.Y = StaticMeshComp->GetRelativeLocation().Y + (DeltaTime * (DistanceBetweenLanes / TurnSpeed));
		if (TimeSinceTurnStarted <= TurnSpeed / 2)
		{
			NewRotation.Yaw = StaticMeshComp->GetRelativeRotation().Yaw + (DeltaTime * TurnDegrees);
		}
		else
		{
			NewRotation.Yaw = StaticMeshComp->GetRelativeRotation().Yaw - (DeltaTime * TurnDegrees);
		}
	}

	StaticMeshComp->SetRelativeLocation(NewLocation);
	StaticMeshComp->SetRelativeRotation(NewRotation);
	
	TimeSinceTurnStarted += DeltaTime;
	if (TimeSinceTurnStarted >= TurnSpeed)
	{
		TimeSinceTurnStarted = 0.f;
		bIsTurning = false;
	}
}

void AEVRVehicleMaster::GetMovementCurve()
{
	if (FCurve)
	{
		// Update function
		TurnTimeline->AddInterpFloat(FCurve, InterpFunction, FName("Alpha"));

		// Set the timeline settings
		TurnTimeline->SetLooping(false);
		TurnTimeline->SetIgnoreTimeDilation(true);

		float MinCurveTimeRange;
		FCurve->GetTimeRange(MinCurveTimeRange, MaxCurveTimeRange);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Float Curve was not found.  Check the location in code"));
	}
}

