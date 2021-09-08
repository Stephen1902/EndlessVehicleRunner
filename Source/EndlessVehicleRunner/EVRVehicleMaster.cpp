// Copyright 2021 DME Games


#include "EVRVehicleMaster.h"

// Sets default values
AEVRVehicleMaster::AEVRVehicleMaster()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEVRVehicleMaster::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEVRVehicleMaster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEVRVehicleMaster::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

