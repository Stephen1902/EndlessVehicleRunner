// Copyright 2021 DME Games


#include "EVRMovementComponent.h"

#include "EVRPlayerVehicle.h"

UEVRMovementComponent::UEVRMovementComponent()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryComponentTick.bCanEverTick = true;
}

void UEVRMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacterRef = Cast<AEVRPlayerVehicle>(GetOwner());

	if (!PlayerCharacterRef)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to get Player Character Ref in Movement Component"));
	}
}
