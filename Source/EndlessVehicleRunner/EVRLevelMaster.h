// Copyright 2021 DME Games

#pragma once

#include "CoreMinimal.h"

#include "Components/ArrowComponent.h"
#include "GameFramework/Actor.h"
#include "EVRLevelMaster.generated.h"

UENUM(BlueprintType)
enum class EFloorType : uint8
{
	FT_Straight		UMETA(DisplayName = "Straight"),
	FT_Left			UMETA(DisplayName = "Left"),
	FT_Right		UMETA(DisplayName = "Right")
};

UCLASS()
class ENDLESSVEHICLERUNNER_API AEVRLevelMaster : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEVRLevelMaster();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Game Level")
	USceneComponent* SceneComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Game Level")
	UStaticMeshComponent* MeshCompBase;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Game Level")
	class UArrowComponent* ArrowComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Game Level")
	class UBoxComponent* EndCollision;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Game Level")
	class UBoxComponent* StartCollision;
		
	// A component for spawning an item the player can hit.  Change Yaw to 180 if not to be used.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Game Level")
	class UEVRSpawnArrowComponent* SpawnPointLeft;

	// A component for spawning an item the player can hit.  Change Yaw to 180 if not to be used.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Game Level")
	class UEVRSpawnArrowComponent* SpawnPointCentre;

	// A component for spawning an item the player can hit.  Change Yaw to 180 if not to be used.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Game Level")
	class UEVRSpawnArrowComponent* SpawnPointRight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Game Level")
	UChildActorComponent* OnRoadActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Game Level")
	UChildActorComponent* NewRoadsideActor;

	// Whether the floor type is straight, turns the player left or right
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Game Level")
	EFloorType FloorType;

	// Items derived from the Spawned Items Master class that can be spawned by this level piece
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Game Level")
	TArray<TSubclassOf<class AEVRSpawnMaster>> ItemsToSpawn;

	// Non-Player vehicles to spawn in the level for the player to hit
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Game Level")
	TArray<TSubclassOf<class AEVRVehicleMaster>> NonPlayerVehicles;
	
	// Roadside spawn items
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Game Level")
	TArray<TSubclassOf<class AEVRRoadsideSpawns>> RoadsidePiecesToSpawn;

	// Vehicles that have been turned by this level piece
	UPROPERTY()
	TArray<class AEVRVehicleMaster*> TurnedThisVehicle;  

	void AddVehicleToTurnedVehicleArray(class AEVRVehicleMaster* VehicleIn);
	
	bool SpawnPickup();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBeginOverlapEndComp(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

// Getters
public:	
	FTransform GetNextSpawnPoint() const { return ArrowComp->GetComponentTransform(); };

	void GetSpawnPointInfo(FVector &NewSpawnLocation, FRotator &NewSpawnRotation) const;
	int32 GetNumberOfLanes() const { return NumberOfLanes; }
private:
	// Number of lanes in this piece so the player position is set correctly
	int32 NumberOfLanes;

	void SetArrowAndBoxTransforms();
	
	void GetReferences();

	UPROPERTY()
	class AEVRGameStateBase* GameStateRef;

	UPROPERTY()
	class AEVRVehiclePlayer* PlayerVehicleRef;

	UPROPERTY()
	TArray<FTransform> SpawnPointArray;
	
	void SpawnRoadsidePiece();
	
	float TurnSpawnChance;
	int32 BlockSpawnChance;

	FTimerHandle DestroyTimer;

	void DestroyThisPiece();

	bool bHasTurnedVehicle = false;
};
