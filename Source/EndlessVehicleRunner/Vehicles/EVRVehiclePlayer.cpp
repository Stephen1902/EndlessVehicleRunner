// Copyright 2021 DME Games


#include "EVRVehiclePlayer.h"
#include "GameplayTask.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "EndlessVehicleRunner/EVRLevelMaster.h"
#include "EndlessVehicleRunner/EVRSaveGame.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

AEVRVehiclePlayer::AEVRVehiclePlayer()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArmComp->SetupAttachment(SceneComponent);
	SpringArmComp->TargetArmLength = 1000.f;
	SpringArmComp->SetRelativeLocation(FVector(0.f, 0.f, 110.f));
	SpringArmComp->SetRelativeRotation(FRotator(-20.f, 0.f, 0.f));

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp);
	
	StartingLife = 100.f;
	LifeLostPerSecond = 1.0f;
	DistanceTravelled = 0.f;
	TimeSinceDistanceUpdated = 0.f;
	TimeSinceSpeedoVaried = 0.f;
	HighScore = 0;
	SaveGameSlot = "Slot0";

	bTurnSoundIsPlaying = false;
	bCornerSoundIsPlaying = false;
	bGameOver = false;
}

FText AEVRVehiclePlayer::GetCurrentSpeed()
{
	// Convert m/s speed to MPH (x10 since speed works out to around 6.7MPH)
	int32 SpeedAsMPH = (GetCurrentSpeedAsFloat() / 1000.f ) * 22.369363;

	// Allow for speedometer to vary a little around top speed 
	int32 RandomSpeedDeviation = 0;

	if (GetCurrentSpeedAsFloat() == MovementSpeed)
	{
		if (TimeSinceSpeedoVaried > 0.75f)
		{
			TimeSinceSpeedoVaried = 0.f;
			RandomSpeedDeviation = FMath::RandRange(-1, 1);
		}
		else
		{
			TimeSinceSpeedoVaried += GetWorld()->DeltaTimeSeconds;
		}
	}
	
	SpeedAsMPH += RandomSpeedDeviation;
	// Convert speed to String
	const FString MPHAsInt = FString::FromInt(SpeedAsMPH);
	const int32 StringLength = MPHAsInt.Len();
	
	FString StringToReturn;

	// Format String as needed for correct UI display
	switch (StringLength)
	{
	case 1:
		StringToReturn = "00" + MPHAsInt;
		break;
	case 2:
		StringToReturn = "0" + MPHAsInt;
		break;
	case 3:
	default:
		StringToReturn = MPHAsInt;
		break;
	}
	
	return FText::FromString(StringToReturn);

}

void AEVRVehiclePlayer::SetCurrentSpeed(const float SpeedIn)
{
	CurrentSpeed -= SpeedIn;
	CurrentSpeed = FMath::Clamp(CurrentSpeed, 0.f, MovementSpeed);
}

void AEVRVehiclePlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("TurnLeft", IE_Pressed, this, &AEVRVehicleMaster::TurnLeft);
	PlayerInputComponent->BindAction("TurnRight", IE_Pressed, this, &AEVRVehicleMaster::TurnRight);
	PlayerInputComponent->BindAction("PauseMenu", IE_Pressed, this, &AEVRVehiclePlayer::PauseKeyPressed);
}

void AEVRVehiclePlayer::ChangePlayerLife(const float LifeChangeValue)
{
	CurrentPlayerLife -= LifeChangeValue;
	CurrentPlayerLife = FMath::Clamp(CurrentPlayerLife, -1.0f, StartingLife);

	// Check if the player has run out of life
	if (CurrentPlayerLife <= 0.f && !bGameOver)
	{
		const FText DistanceToBroadcast =  FText::FromString(FString::SanitizeFloat(FMath::Floor(DistanceTravelled), 0) + "M");
		FText HighScoreToBroadcast =  FText::FromString(FString::FromInt(HighScore) + "M");
		
		// Check if player has set a new distance record
		if (DistanceTravelled > HighScore)
		{
			HighScore = DistanceTravelled;
			if (ThisSaveGameRef)
			{
				ThisSaveGameRef->SavedHighScore = HighScore;
				
				if (UGameplayStatics::SaveGameToSlot(ThisSaveGameRef, SaveGameSlot, 0))
				{
					UE_LOG(LogTemp, Warning, TEXT("Game Saved"));
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Game failed to save"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("ThisSaveGameRef not valid"));
			}

			HighScoreToBroadcast =  FText::FromString(FString::FromInt(HighScore) + "M");
			OnGameOver.Broadcast(DistanceToBroadcast, HighScoreToBroadcast, true);
		}
		else
		{
			OnGameOver.Broadcast(DistanceToBroadcast, HighScoreToBroadcast, false);
		}

		ShowMouseAndLockDisplay();
		
		// Pause the game
		UGameplayStatics::SetGamePaused(GetWorld(), true);
		bGameOver = true;
	}
}

void AEVRVehiclePlayer::BeginPlay()
{
	Super::BeginPlay();
	
	AddPlayerWidgetToScreen();
	SetSaveGameReferences();
	ShowMouseAndLockDisplay();	
	
	CurrentPlayerLife = StartingLife;

	YLocations.Add(GetActorLocation().Y - DistanceBetweenLanes);
	YLocations.Add(GetActorLocation().Y);
	YLocations.Add(GetActorLocation().Y + DistanceBetweenLanes);

	UMaterialInterface* CurrentMaterial = StaticMeshComp->GetMaterial(0);
	DynMaterial = UMaterialInstanceDynamic::Create(CurrentMaterial, this);
	StaticMeshComp->SetMaterial(0, DynMaterial);

	if (EngineSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), EngineSound);
	}

	UGameplayStatics::SetGamePaused(GetWorld(), true);
	bGameOver = false;
}

void AEVRVehiclePlayer::Turn(const float DeltaTime)
{
	Super::Turn(DeltaTime);

	// If the player is going at over 75% of the vehicle's max speed, play the sound
	if (TurnAtSpeedSound  && CurrentSpeed > (GetMaxSpeed() * 0.90f))
	{
		// Check that the sound isn't already playing
		if (!bTurnSoundIsPlaying && !bCornerSoundIsPlaying)
		{
			bTurnSoundIsPlaying = true;
			UGameplayStatics::PlaySound2D(GetWorld(), TurnAtSpeedSound);
		}
	}

	if (!bIsTurning)
	{
		bTurnSoundIsPlaying = false;
	}
}

void AEVRVehiclePlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ChangePlayerLife(DeltaTime * LifeLostPerSecond);
	PlayRevvingSound();
	AddToDistanceTravelled(DeltaTime);
}

void AEVRVehiclePlayer::AddPlayerWidgetToScreen()
{
	if (PlayerWidget)
	{
		UUserWidget* WidgetToCreate = CreateWidget(GetWorld(), PlayerWidget);
		WidgetToCreate->AddToViewport();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Widget not set for EVRPlayeVehicle"));
	}
}

void AEVRVehiclePlayer::TimelineFloatReturn(float Value)
{
	Super::TimelineFloatReturn(Value);

	// If the player is going at over 74% of the vehicle's max speed, play the sound
	if (TurnRoundCornerSound && CurrentSpeed > (GetMaxSpeed() * 0.74f))
	{
		// Check that a sound isn't already playing
		if (!bTurnSoundIsPlaying && !bCornerSoundIsPlaying)
		{
			bCornerSoundIsPlaying = true;
			UGameplayStatics::PlaySound2D(GetWorld(), TurnRoundCornerSound);
		}
	}

	// Reset the turn variable when reaching the end of the timeline
	if (Value >= 0.99f)
	{
		bCornerSoundIsPlaying = false;
	}
	
	// Bring the camera in (as if braking) in the first half of the timeline then move it out again
	if (Value <= 0.5f)
	{
		DynMaterial->SetScalarParameterValue("BrakeParam", 1.f);
		SpringArmComp->TargetArmLength = (FMath::Lerp(1000.f, 500.f, Value * 2));
	}
	else
	{
		DynMaterial->SetScalarParameterValue("BrakeParam", 0.f);
		SpringArmComp->TargetArmLength = (FMath::Lerp(500.f, 1000.f, (Value -0.5f) * 2));
	}	
}

void AEVRVehiclePlayer::PlayRevvingSound() const
{
	const float SpeedAsPercentage = 1.0f - ((GetMaxSpeed() - CurrentSpeed) / GetMaxSpeed());
	const bool bShouldRev = FMath::IsNearlyEqual(SpeedAsPercentage, 0.15f, 0.f) || FMath::IsNearlyEqual(SpeedAsPercentage, 0.35f, 0.f) || FMath::IsNearlyEqual(SpeedAsPercentage, 0.60f, 0.f);
	
	if (EngineNoteSound && bShouldRev)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), EngineNoteSound);
	}
}

void AEVRVehiclePlayer::AddToDistanceTravelled(float TimeIn)
{
	// Convert m/s speed to MPH (x10 since speed works out to around 6.7MPH)
	const int32 SpeedAsMPH = (GetCurrentSpeedAsFloat() / 1000.f ) * 22.369363;

	DistanceTravelled += SpeedAsMPH * TimeIn;

	TimeSinceDistanceUpdated += TimeIn;

	if (TimeSinceDistanceUpdated > 0.08f)
	{
		TimeSinceDistanceUpdated = 0.f;
		
		const FText TextToBroadcast =  FText::FromString(FString::SanitizeFloat(FMath::Floor(DistanceTravelled), 0));
		OnDistanceUpdated.Broadcast(TextToBroadcast);
	}
	
}

void AEVRVehiclePlayer::PauseKeyPressed()
{
	OnPauseKeyPressed.Broadcast();
	ShowMouseAndLockDisplay();
	UGameplayStatics::SetGamePaused(GetWorld(), true);
}

void AEVRVehiclePlayer::SetSaveGameReferences()
{
	// Check if a saved game already exists and if not, create it
	if (UGameplayStatics::DoesSaveGameExist(SaveGameSlot, 0))
	{
		SaveGameRef = UGameplayStatics::LoadGameFromSlot(SaveGameSlot, 0);
	}
	else
	{
		SaveGameRef = Cast<UEVRSaveGame>(UGameplayStatics::CreateSaveGameObject(UEVRSaveGame::StaticClass()));
	}

	// Cast to the specific instance of the SaveGame class for the HighScore variable
	ThisSaveGameRef = Cast<UEVRSaveGame>(SaveGameRef);
	
	if (ThisSaveGameRef)
	{
		HighScore = ThisSaveGameRef->SavedHighScore;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to get UEVRHighScore in EVRVehiclePlayer"));
	}

}

void AEVRVehiclePlayer::ShowMouseAndLockDisplay() const
{
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->SetShowMouseCursor(true);
		PC->SetInputMode(FInputModeUIOnly());
	}

}
