// Copyright 2021 DME Games

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EVRSpawnMaster.generated.h"

UCLASS()
class ENDLESSVEHICLERUNNER_API AEVRSpawnMaster : public AActor
{
	GENERATED_BODY()
public:
	// Sets default values for this actor's properties
	AEVRSpawnMaster();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawned Items")
	USceneComponent* SceneComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawned Items")
	UStaticMeshComponent* StaticMeshComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawned Items")
	UParticleSystem* EndOfLifeParticle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawned Items")
	USoundBase* EndOfLifeSound; 
	
	// Sets whether or not the spawned actor can move
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawned Item Properties")
	bool bCanMove;

	// Percentage of player speed this object can move
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="bCanMove", ClampMin=0.1, ClampMax=1.0), Category = "Spawned Item Properties")
	float MovementSpeed;

	// Amount of health, as a percentage, this actor gives the player when hit
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=1, ClampMax=100), Category = "Spawned Item Properties")
	float PlayerHealthRestored;

	// Amount of speed, as a percentage, this actor slows the player vehicle when hit
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=1, ClampMax=100), Category = "Spawned Item Properties")
	float PlayerSpeedDecrease;

	bool GetIsBeingTurned() const { return bIsBeingTurned; }
	void SetIsBeingTurned(const bool TurnedIn) { bIsBeingTurned = TurnedIn; } 

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY()
	class AEVRVehiclePlayer* PlayerVehicleRef;

	UPROPERTY()
	class AEVRGameStateBase* GameStateRef;

	void SetReferences();

	void MoveSpawnedActor(float DeltaTime);

	UFUNCTION()
	void OnBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	float InternalMovementSpeed;
	float SpeedOfAcceleration = 0.f;
	float CurrentSpeed = 0.f;

	bool bIsBeingTurned;
};
