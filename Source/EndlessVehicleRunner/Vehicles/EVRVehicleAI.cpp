// Copyright 2021 DME Games


#include "EVRVehicleAI.h"
#include "EVRVehiclePlayer.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

AEVRVehicleAI::AEVRVehicleAI()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FrontOfVehicleCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Vehicle Front Collision"));
	FrontOfVehicleCollision->SetupAttachment(StaticMeshComp);
	FrontOfVehicleCollision->SetBoxExtent(FVector(64.f, 32.f, 32.f), false);
	FrontOfVehicleCollision->SetHiddenInGame(false);
	
	StaticMeshComp->OnComponentBeginOverlap.AddDynamic(this, &AEVRVehicleAI::OnBeginOverlap);
}

void AEVRVehicleAI::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(20.f);
	SetReferences();
	SetLocationOfFrontCollision();
}

void AEVRVehicleAI::OnBeginOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (PlayerVehicleRef && OtherActor == PlayerVehicleRef)
	{
		const float PlayerLifeToRestore = PlayerVehicleRef->GetStartingLife() * (PlayerHealthRestored / 100.f);
		PlayerVehicleRef->ChangePlayerLife(-PlayerLifeToRestore);

		float PlayerSpeedToReduce = PlayerVehicleRef->GetMaxSpeed() * (PlayerSpeedDecrease / 100.f);

		// Calculate if player hits a moving spawn from behind or from the side and adjust speed accordingly.
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

void AEVRVehicleAI::OnFrontCollisionOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AEVRVehicleAI* VehicleHit = Cast<AEVRVehicleAI>(OtherActor))
	{
		
	}
}

void AEVRVehicleAI::SetReferences()
{
	PlayerVehicleRef = Cast<AEVRVehiclePlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
}

void AEVRVehicleAI::SetLocationOfFrontCollision()
{
	//	const UStaticMesh* CurrentMesh = StaticMeshComp->GetStaticMesh();
	//	float NewX = CurrentMesh->GetBounds().GetBox().GetSize().X;
	FVector LocalMin;
	FVector LocalMax;
	StaticMeshComp->GetLocalBounds(LocalMin, LocalMax);
	
	const float NewBoxLocation = (LocalMax.X + 68.f);
	
	FrontOfVehicleCollision->SetRelativeLocation(FVector(NewBoxLocation, 0.f, 50.f));

}