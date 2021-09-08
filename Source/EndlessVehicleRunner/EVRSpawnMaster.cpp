// Copyright 2021 DME Games


#include "EVRSpawnMaster.h"
#include "EVRGameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Player/EVRPlayerVehicle.h"

// Sets default values
AEVRSpawnMaster::AEVRSpawnMaster()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	SetRootComponent(SceneComp);

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	StaticMeshComp->SetupAttachment(SceneComp);

	StaticMeshComp->OnComponentBeginOverlap.AddDynamic(this, &AEVRSpawnMaster::OnBeginOverlap);

	bCanMove = false;
	MovementSpeed = 0.f;
	PlayerHealthRestored = 1.0f;
	PlayerSpeedDecrease = 1.f;
	InternalMovementSpeed = 0.5f;

}

// Called when the game starts or when spawned
void AEVRSpawnMaster::BeginPlay()
{
	Super::BeginPlay();

	SetReferences();

	if (bCanMove && PlayerVehicleRef)
	{
		InternalMovementSpeed = PlayerVehicleRef->GetMaxSpeed() * MovementSpeed;
	}
}

// Called every frame
void AEVRSpawnMaster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveSpawnedActor(DeltaTime);
}

void AEVRSpawnMaster::SetReferences()
{
	GameStateRef = Cast<AEVRGameStateBase>(GetWorld()->GetGameState());
	PlayerVehicleRef = Cast<AEVRPlayerVehicle>(GetWorld()->GetFirstPlayerController()->GetPawn());

	if (PlayerVehicleRef)
	{
		SpeedOfAcceleration = PlayerVehicleRef->GetSpeedOfAcceleration();
	}
}

void AEVRSpawnMaster::MoveSpawnedActor(float DeltaTime)
{
	if (bCanMove && InternalMovementSpeed > 0.f)
	{
		if (CurrentSpeed < MovementSpeed)
		{
			CurrentSpeed += MovementSpeed / (MovementSpeed / SpeedOfAcceleration);
		}
		
		const FVector MoveDirection = FVector(InternalMovementSpeed * DeltaTime, 0.f, 0.f);
		AddActorLocalOffset(MoveDirection, true);
	}
}


void AEVRSpawnMaster::OnBeginOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (PlayerVehicleRef && OtherActor == PlayerVehicleRef)
	{
		const float PlayerLifeToRestore = PlayerVehicleRef->GetStartingLife() * (PlayerHealthRestored / 100.f);
		PlayerVehicleRef->ChangePlayerLife(-PlayerLifeToRestore);

		float PlayerSpeedToReduce = PlayerVehicleRef->GetMaxSpeed() * (PlayerSpeedDecrease / 100.f);

		// Calculate if player hits a moving spawn from behind or from the side and adjust speed accordingly.
		if (bCanMove)
		{
			const FVector PlayerCurrentPosition = PlayerVehicleRef->StaticMeshComp->GetComponentLocation();
			const FVector SpawnCurrentPosition = GetActorLocation();
	
			// Check to see if the player is travelling along the X or Y Axis.
			if (PlayerVehicleRef->GetActorForwardVector().X != 1.f)
			{
				// Check whether the Y locations match.  If not, the hit is on the side and the speed loss is reduced.
				if (!FMath::IsNearlyEqual(PlayerCurrentPosition.Y, SpawnCurrentPosition.Y, 1.0f))
				{
					PlayerSpeedToReduce *= .5f;					
				}
			}
			else
			{
				// Check whether the X locations match.  If not, the hit is on the side and the speed loss is reduced.
				if (!FMath::IsNearlyEqual(PlayerCurrentPosition.X, SpawnCurrentPosition.X, 1.0f))
				{
					PlayerSpeedToReduce *= .5f;					
				}
			}
		}
		
		PlayerVehicleRef->SetCurrentSpeed(PlayerSpeedToReduce);
		
		if (EndOfLifeParticle)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EndOfLifeParticle, GetActorLocation(), GetActorRotation());
		}

		if (EndOfLifeSound)
		{
			UGameplayStatics::PlaySound2D(GetWorld(), EndOfLifeSound);	
		}
		
		Destroy();
	}
}
