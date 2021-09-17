// Copyright 2021 DME Games

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "EVRMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class ENDLESSVEHICLERUNNER_API UEVRMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

	UEVRMovementComponent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
/**
private:
	UPROPERTY()
	class AEVRVehiclePlayer* PlayerCharacterRef;
*/
};
