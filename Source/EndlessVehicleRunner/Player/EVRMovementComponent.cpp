// Copyright 2021 DME Games


#include "EVRMovementComponent.h"
#include "../Vehicles/EVRVehiclePlayer.h"

class AEVRVehiclePlayer;

UEVRMovementComponent::UEVRMovementComponent()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryComponentTick.bCanEverTick = true;
}

void UEVRMovementComponent::BeginPlay()
{
	Super::BeginPlay();

/**	PlayerCharacterRef = Cast<AEVRVehiclePlayer>(GetOwner());

	if (!PlayerCharacterRef)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to get Player Character Ref in Movement Component"));
	}
*/
}
