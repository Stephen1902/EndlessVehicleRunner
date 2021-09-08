	// Copyright 2021 DME Games


	#include "EVRGameStateBase.h"
	#include "EVRLevelMaster.h"

	AEVRGameStateBase::AEVRGameStateBase()
	{
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		CreateTurn = 0;
	}

	void AEVRGameStateBase::SpawnStartLevel()
	{
		if (StraightPiecesToSpawn.Num() > 0)
		{
			for (int32 i = 0; i < 10; ++i)
			{
				SpawnNextLevelPiece(true, 0);
			}
		}
	}

	void AEVRGameStateBase::SpawnNextLevelPiece(const bool SpawnStraight,const int32 PieceToSpawn)
	{
		AEVRLevelMaster* NewLevelPiece;
		
		if (SpawnStraight)
		{
			NewLevelPiece = GetWorld()->SpawnActor<AEVRLevelMaster>(StraightPiecesToSpawn[PieceToSpawn], SpawnTransform, SpawnInfo);
		}
		else
		{
			NewLevelPiece = GetWorld()->SpawnActor<AEVRLevelMaster>(TurnPiecesToSpawn[PieceToSpawn], SpawnTransform, SpawnInfo);
		}
		
		SpawnTransform = NewLevelPiece->GetNextSpawnPoint();
		NewLevelPiece->GetSpawnPointInfo(SpawnLocation, SpawnRotation);
	}

	void AEVRGameStateBase::TryToSpawnPickup()
	{
		if (LevelMasterRef)
		{
			if (FMath::RandRange(0, 75) < SpawnPickupChance && LevelMasterRef->SpawnPickup())
			{
				SpawnPickupChance = 0;
			}
		}
	}

	void AEVRGameStateBase::TryToSpawnNextPiece()
	{
		SpawnPickupChance += 25;
		CreateTurn += 1;
		bool CreatedTurn = false;

		// If more than 8 straight pieces have been spawned, chance of a turn occuring
		if (CreateTurn > 8)
		{
			// Always turn if 15 straights in a row
			if (FMath::RandRange(0, 99) > 49 || CreateTurn > 15)
			{
				CreateTurn = 0;
				const int32 RandomSpawn = FMath::RandRange(0, TurnPiecesToSpawn.Num() - 1);
				SpawnNextLevelPiece(false, RandomSpawn);
				CreatedTurn = true;
			}
		}

		// A turn wasn't created, create a straight
		if (CreatedTurn == false)
		{
			// Don't allow for the possibility of 2 non-special straight pieces to spawn consecutively
			if (FMath::RandRange(25, 124) < SpecialStraightSpawnChance)
			{
				SpecialStraightSpawnChance = 0;
				const int32 RandomSpawn = FMath::RandRange(1, StraightPiecesToSpawn.Num() - 1);
				SpawnNextLevelPiece(true, RandomSpawn);
			}
			else
			{
				SpawnNextLevelPiece(true, 0);
				SpecialStraightSpawnChance += 25;
			}

			// As it's a straight piece, try to spawn a pickup on it
			TryToSpawnPickup();
		}
	}

	void AEVRGameStateBase::SetCurrentPieceYaw(const float NewYaw)
	{
		CurrentPieceYaw = NewYaw;
	}

	void AEVRGameStateBase::BeginPlay()
	{
		Super::BeginPlay();

		SpawnStartLevel();
	}


