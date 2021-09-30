// Copyright 2021 DME Games

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "EVRSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class ENDLESSVEHICLERUNNER_API UEVRSaveGame : public USaveGame
{
	GENERATED_BODY()

	
public:
	UEVRSaveGame();
	
	UPROPERTY(EditAnywhere, Category = "Saved Game Options")
	int32 SavedHighScore;

	
};
