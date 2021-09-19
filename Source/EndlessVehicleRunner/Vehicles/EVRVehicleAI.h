// Copyright 2021 DME Games

#pragma once

#include "CoreMinimal.h"
#include "EVRVehicleMaster.h"
#include "EVRVehicleAI.generated.h"

/**
 * 
 */
UCLASS()
class ENDLESSVEHICLERUNNER_API AEVRVehicleAI : public AEVRVehicleMaster
{
	GENERATED_BODY()

public:
	AEVRVehicleAI();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawned Vehicle")
	UParticleSystem* EndOfLifeParticle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawned Vehicle")
	USoundBase* EndOfLifeSound;

	// Sound to play when another vehicle intersects with the box collision
	UPROPERTY(Category = "Spawned Vehicle", EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	USoundBase* HornSound;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawned Vehicle")
	class UBoxComponent* FrontOfVehicleCollision;

	// Amount of health, as a percentage, this actor gives the player when hit
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=1, ClampMax=100), Category = "Spawned Vehicle Properties")
	float PlayerHealthRestored;

	// Amount of speed, as a percentage, this actor slows the player vehicle when hit
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=1, ClampMax=100), Category = "Spawned Vehicle Properties")
	float PlayerSpeedDecrease;

	// Helper function to ensure vehicle turns correctly when a corner is reached
	void SetHasTurned(const bool TurnedIn) { bHasTurned = TurnedIn; }
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
private:
	UFUNCTION()
	void OnBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OnFrontCollisionOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UPROPERTY()
	class AEVRVehiclePlayer* PlayerVehicleRef;
	
	void SetReferences();
	void SetLocationOfFrontCollision() const;
};
