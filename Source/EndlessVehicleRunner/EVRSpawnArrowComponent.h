// Copyright 2021 DME Games

#pragma once

#include "CoreMinimal.h"
#include "Components/ArrowComponent.h"
#include "EVRSpawnArrowComponent.generated.h"

/**
 * 
 */
UCLASS()
class ENDLESSVEHICLERUNNER_API UEVRSpawnArrowComponent : public UArrowComponent
{
	GENERATED_BODY()

public:
	UEVRSpawnArrowComponent();

	bool GetCanSpawnHere() const { return bCanSpawnHere; }
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Spawn Option")
	bool bCanSpawnHere;	
};
