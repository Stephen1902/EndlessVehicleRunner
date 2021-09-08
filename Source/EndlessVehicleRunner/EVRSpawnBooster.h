// Copyright 2021 DME Games

#pragma once

#include "CoreMinimal.h"
#include "EVRSpawnMaster.h"
#include "EVRSpawnBooster.generated.h"

/**
 * 
 */
UCLASS()
class ENDLESSVEHICLERUNNER_API AEVRSpawnBooster : public AEVRSpawnMaster
{
	GENERATED_BODY()
public:
	AEVRSpawnBooster();

	UPROPERTY(EditDefaultsOnly, Category = "Booster Setup")
	FLinearColor MeshColour;

	UPROPERTY(EditDefaultsOnly, Category = "Booster Setup")
	float PulseFrequency;

	UPROPERTY(EditDefaultsOnly, Category = "Booster Setup")
	float PulseEmission;

	UPROPERTY(EditDefaultsOnly, Category = "Booster Setup")
	bool bRotates;

	UPROPERTY(EditDefaultsOnly, Category = "Booster Setup", meta=(EditCondition="bRotates"))
	float RotateSpeed;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
private:
	UPROPERTY()
	class UMaterial* FlatMaterial;

	UPROPERTY()
	class UMaterial* PulseMaterial;
};
