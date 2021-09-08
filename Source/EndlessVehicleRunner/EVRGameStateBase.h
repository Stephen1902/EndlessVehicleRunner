// Copyright 2021 DME Games

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "EVRGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class ENDLESSVEHICLERUNNER_API AEVRGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

	AEVRGameStateBase();
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Level Pieces")
	TArray<TSubclassOf<class AEVRLevelMaster>> StraightPiecesToSpawn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Level Pieces")
	TArray<TSubclassOf<class AEVRLevelMaster>> TurnPiecesToSpawn;

	void TryToSpawnNextPiece();

	void SetCurrentPieceYaw(const float NewYaw);

	FRotator GetCurrentRotation() const { return SpawnRotation; }

	void SetEVRLevelMasterRef(class AEVRLevelMaster* ReferenceIn) { LevelMasterRef = ReferenceIn; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	FTransform SpawnTransform;
// Spawn Properties
	FVector SpawnLocation;
	FRotator SpawnRotation;
	FActorSpawnParameters SpawnInfo;
	
	void SpawnStartLevel();
	void SpawnNextLevelPiece(const bool SpawnStraight, const int32 PieceToSpawn);
	void TryToSpawnPickup();

	int32 CreateTurn;

	float CurrentPieceYaw;

	int32 SpecialStraightSpawnChance = 0;
	int32 SpawnPickupChance = 0;

	UPROPERTY()
	class AEVRLevelMaster* LevelMasterRef;

};
