// Copyright 2021 DME Games


#include "EVRPlayerVehicle.h"
#include "EVRMovementComponent.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "EndlessVehicleRunner/EVRGameStateBase.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
AEVRPlayerVehicle::AEVRPlayerVehicle()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(SceneComponent);
	
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Comp"));
	StaticMeshComp->SetMobility(EComponentMobility::Movable);
	StaticMeshComp->SetupAttachment(SceneComponent);

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArmComp->SetupAttachment(SceneComponent);
	SpringArmComp->TargetArmLength = 1000.f;
	SpringArmComp->SetRelativeLocation(FVector(0.f, 0.f, 110.f));
	SpringArmComp->SetRelativeRotation(FRotator(-20.f, 0.f, 0.f));

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp);

	MovementComponent = CreateDefaultSubobject<UEVRMovementComponent>(TEXT("Movement Component"));
	//MovementComponent->SetAutoActivate(true);

	TurnTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Timeline"));
	
	static ConstructorHelpers::FObjectFinder<UCurveFloat> CurveFloatObject(TEXT("CurveFloat'/Game/Blueprints/Player/FloatCurveNormal.FloatCurveNormal'"));
	FCurve = CurveFloatObject.Object;

	InterpFunction.BindUFunction(this, FName("TimelineFloatReturn"));
	TimelineFinished.BindUFunction(this, FName("OnTimelineFinished"));

	bGameIsPaused = false;
	bIsTurning = false;
	bCanTurn = false;
	MovementSpeed = 500.f;
	SpeedOfAcceleration = 5.0f;
	DistanceBetweenLanes = 200.f;
	TurnSpeed = 0.25f;
	TurnDegrees = 100.f;
	DesiredRotation = 180.f;
	LocationOffset = 0.f;
	
	TimeSinceTurnStarted = 0.f;
	LocationIndex = 1;

	StartingLife = 100.f;
	LifeLostPerSecond = 1.0f;

	TimeSinceSpeedoVaried = 0.f;
}

FText AEVRPlayerVehicle::GetCurrentSpeed()
{
	// Convert m/s speed to MPH (x10 since speed works out to around 6.7MPH)
	int32 SpeedAsMPH = (CurrentSpeed / 1000.f ) * 22.369363;

	// Allow for speedometer to vary a little around top speed 
	int32 RandomSpeedDeviation = 0;

	if (CurrentSpeed == MovementSpeed)
	{
		if (TimeSinceSpeedoVaried > 0.75f)
		{
			TimeSinceSpeedoVaried = 0.f;
			RandomSpeedDeviation = FMath::RandRange(-1, 1);
		}
		else
		{
			TimeSinceSpeedoVaried += GetWorld()->DeltaTimeSeconds;
		}
	}
	
	SpeedAsMPH += RandomSpeedDeviation;
	// Convert speed to String
	const FString MPHAsInt = FString::FromInt(SpeedAsMPH);
	const int32 StringLength = MPHAsInt.Len();
	
	FString StringToReturn;

	// Format String as needed for correct UI display
	switch (StringLength)
	{
		case 1:
			StringToReturn = "00" + MPHAsInt;
			break;
		case 2:
			StringToReturn = "0" + MPHAsInt;
			break;
		case 3:
		default:
			StringToReturn = MPHAsInt;
			break;
	}
	
	return FText::FromString(StringToReturn);
}

void AEVRPlayerVehicle::SetCurrentSpeed(const float SpeedIn)
{
	CurrentSpeed -= SpeedIn;
	CurrentSpeed = FMath::Clamp(CurrentSpeed, 0.f, MovementSpeed);
}

void AEVRPlayerVehicle::SetNewLocation(bool MoveXIn, bool MoveYIn, float LocationIn)
{
	
}

// Called when the game starts or when spawned
void AEVRPlayerVehicle::BeginPlay()
{
	Super::BeginPlay();

	AddPlayerWidgetToScreen();
	GetMovementCurve();
	
	CurrentSpeed = 0.f;
	CurrentPlayerLife = StartingLife;

	YLocations.Add(GetActorLocation().Y - DistanceBetweenLanes);
	YLocations.Add(GetActorLocation().Y);
	YLocations.Add(GetActorLocation().Y + DistanceBetweenLanes);
}

void AEVRPlayerVehicle::TimelineFloatReturn(float Value)
{
	CurrentSpeed = MovementSpeed * 0.75f;
	
	SetActorLocation(FMath::Lerp(StartPlayerLocation, EndPlayerLocation, Value));
	SetActorRotation(FMath::Lerp(StartPlayerRotation, EndPlayerRotation, Value));
}

void AEVRPlayerVehicle::OnTimelineFinished()
{
	
}

// Called every frame
void AEVRPlayerVehicle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!bGameIsPaused)
	{
		MoveVehicleForward(DeltaTime);
		ChangePlayerLife(DeltaTime * LifeLostPerSecond);
		
		if (bIsTurning)
		{
			Turn(DeltaTime);
		}
	}
	
}

// Called to bind functionality to input
void AEVRPlayerVehicle::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("TurnLeft", IE_Pressed, this, &AEVRPlayerVehicle::TurnLeft);
	PlayerInputComponent->BindAction("TurnRight", IE_Pressed, this, &AEVRPlayerVehicle::TurnRight);
}

void AEVRPlayerVehicle::AddPlayerWidgetToScreen()
{
	if (PlayerWidget)
	{
		UUserWidget* WidgetToCreate = CreateWidget(GetWorld(), PlayerWidget);
		WidgetToCreate->AddToViewport();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Widget not set for EVRPlayeVehicle"));
	}
}

void AEVRPlayerVehicle::MoveVehicleForward(const float DeltaTime)
{
	if (CurrentSpeed < MovementSpeed)
	{
		CurrentSpeed += MovementSpeed / (MovementSpeed / SpeedOfAcceleration);
	}

	const FVector MoveDirection = FVector(CurrentSpeed * DeltaTime, 0.f, 0.f);
	AddActorLocalOffset(MoveDirection, true);
}

void AEVRPlayerVehicle::ChangePlayerLife(const float LifeChangeValue)
{
	CurrentPlayerLife -= LifeChangeValue;
	CurrentPlayerLife = FMath::Clamp(CurrentPlayerLife, -1.0f, StartingLife);
}

void AEVRPlayerVehicle::TurnLeft()
{
	if (!bCanTurn && LocationIndex > 0 && !bIsTurning)
	{
		LastLocationIndex = LocationIndex;
		LocationIndex -= 1;
		bIsTurning = true;
	}
}

void AEVRPlayerVehicle::TurnRight()
{
	if (!bCanTurn && LocationIndex < 2 && !bIsTurning)
	{
		LastLocationIndex = LocationIndex;
		LocationIndex += 1;
		bIsTurning = true;
	}
}

void AEVRPlayerVehicle::Turn(const float DeltaTime)
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

		StaticMeshComp->SetRelativeLocation(FVector(NewLocation.X, YLocations[LocationIndex], NewLocation.Z));
		StaticMeshComp->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
	}
}

void AEVRPlayerVehicle::GetMovementCurve()
{
	if (FCurve)
	{
		// Update function
		TurnTimeline->AddInterpFloat(FCurve, InterpFunction, FName("Alpha"));
		//TurnTimeline->AddInterpFloat(FCurve, InterpFunction, FName("Alpha"));
		// OnFinished function
		TurnTimeline->SetTimelineFinishedFunc(TimelineFinished);

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

void AEVRPlayerVehicle::SetNewPositionAfterTurn(const FVector ArrowLocation, const FRotator NewPlayerRotation)
{
//	const FVector NewLoc = GetActorLocation();
	StartPlayerLocation = GetActorLocation();
	StartPlayerRotation = GetActorRotation();
	EndPlayerRotation = NewPlayerRotation;
	
	if (!bHasTurned)
	{
		//SceneComponent->SetWorldLocation(FVector(ArrowLocation.X, NewLoc.Y, NewLoc.Z));
		EndPlayerLocation = FVector(ArrowLocation.X, StartPlayerLocation.Y, StartPlayerLocation.Z);
		
		bHasTurned = true;
	}
	else
	{
//		SceneComponent->SetWorldLocation(FVector(NewLoc.X, ArrowLocation.Y, NewLoc.Z));

		EndPlayerLocation = FVector(StartPlayerLocation.X, ArrowLocation.Y, StartPlayerLocation.Z);
		bHasTurned = false;
	}

	if (FCurve)
	{
		TurnTimeline->PlayFromStart();
	}
}
