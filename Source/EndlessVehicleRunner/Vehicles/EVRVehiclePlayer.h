// Copyright 2021 DME Games

#pragma once

#include "CoreMinimal.h"
#include "EVRVehicleMaster.h"
#include "EVRVehiclePlayer.generated.h"

/**
 * 
 */
UCLASS()
class ENDLESSVEHICLERUNNER_API AEVRVehiclePlayer : public AEVRVehicleMaster
{
	GENERATED_BODY()
public:
	AEVRVehiclePlayer();
	
	UPROPERTY(Category = "Player Vehicle", EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArmComp;

	UPROPERTY(Category = "Player Vehicle", EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	class UCameraComponent* CameraComp;

	UPROPERTY(Category = "Player Vehicle", EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> PlayerWidget;
	
	// Sound to play when the player changed lane at high speed 
	UPROPERTY(Category = "Player Vehicle", EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	class USoundBase* TurnAtSpeedSound;

	// Sound to play when the player turns a corner
	UPROPERTY(Category = "Player Vehicle", EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	class USoundBase* TurnRoundCornerSound;

	// Generic Engine sound to play
	UPROPERTY(Category = "Player Vehicle", EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	class USoundBase* EngineSound;

	// Engine revving / gear changing sound
	UPROPERTY(Category = "Player Vehicle", EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	class USoundBase* EngineNoteSound;
	
	// Starting Life of the player vehicle
	UPROPERTY(EditDefaultsOnly, Category = "Player Vehicle Options")
	float StartingLife;

	// Amount of life the player vehicle loses
	UPROPERTY(EditDefaultsOnly, Category = "Player Vehicle Options")
	float LifeLostPerSecond;

	UFUNCTION(BlueprintCallable, Category = "Player Vehicle Gameplay")
	float GetStartingLife() const { return StartingLife; }

	UFUNCTION(BlueprintCallable, Category = "Player Vehicle Gameplay")
	FText GetCurrentSpeed();
	
	UFUNCTION(BlueprintCallable, Category = "Player Vehicle Gameplay")
	void SetCurrentSpeed(const float SpeedIn);
	
	UFUNCTION(BlueprintCallable, Category = "Player Vehicle Gameplay")
	float GetCurrentLife() const { return 1.f - ((StartingLife - CurrentPlayerLife) / StartingLife); }
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void ChangePlayerLife(const float LifeChangeValue);

	float GetSpeedOfAcceleration() const { return SpeedOfAcceleration; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Turn(const float DeltaTime) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
private:
	float CurrentPlayerLife;
	
	// To quickly flash the player speed, making it look like it's varying slightly
	float TimeSinceSpeedoVaried;

	void AddPlayerWidgetToScreen();
	
	UPROPERTY()
	UMaterialInstanceDynamic* DynMaterial;

	virtual void TimelineFloatReturn(float Value) override;

	bool bTurnSoundIsPlaying;
	bool bCornerSoundIsPlaying;

	void PlayRevvingSound() const;

};

