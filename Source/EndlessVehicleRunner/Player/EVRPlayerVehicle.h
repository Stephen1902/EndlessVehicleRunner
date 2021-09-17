// Copyright 2021 DME Games

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/TimelineComponent.h"
#include "Math/UnitConversion.h"
#include "EVRPlayerVehicle.generated.h"

UCLASS()
class ENDLESSVEHICLERUNNER_API AEVRPlayerVehicle : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AEVRPlayerVehicle();

	UPROPERTY(Category = "Player Vehicle", EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	USceneComponent* SceneComponent;
	
	UPROPERTY(Category = "Player Vehicle", EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMeshComp;

	UPROPERTY(Category = "Player Vehicle", EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArmComp;

	UPROPERTY(Category = "Player Vehicle", EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	class UCameraComponent* CameraComp;

	UPROPERTY(Category = "Player Vehicle", EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	class UEVRMovementComponent* MovementComponent;

	UPROPERTY(Category = "Player Vehicle", EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> PlayerWidget;

	// Distance between lanes
	UPROPERTY(EditDefaultsOnly, Category = "Player Vehicle Options")
	float DistanceBetweenLanes;
	
	// Units in cm the player vehicle will move forward per second
	UPROPERTY(EditDefaultsOnly, Category = "Player Vehicle Options")
	float MovementSpeed;

	// Time in seconds it takes for the vehicle to reach top speed
	UPROPERTY(EditDefaultsOnly, Category = "Player Vehicle Options")
	float SpeedOfAcceleration;
	
	// Time in seconds it takes to move the vehicle to another lane 
	UPROPERTY(EditDefaultsOnly, Category = "Player Vehicle Options")
	float TurnSpeed;

	// Amount of visible "turn" in the vehicle when changing lanes
	UPROPERTY(EditDefaultsOnly, Category = "Player Vehicle Options")
	float TurnDegrees;
	
	// Starting Life of the player vehicle
	UPROPERTY(EditDefaultsOnly, Category = "Player Vehicle Options")
	float StartingLife;

	// Amount of life the player vehicle loses
	UPROPERTY(EditDefaultsOnly, Category = "Player Vehicle Options")
	float LifeLostPerSecond;

	UPROPERTY()
	UTimelineComponent* TurnTimeline;

	UFUNCTION(BlueprintCallable, Category = "Player Vehicle Gameplay")
	float GetStartingLife() const { return StartingLife; }

	UFUNCTION(BlueprintCallable, Category = "Player Vehicle Gameplay")
	float GetMaxSpeed() const { return MovementSpeed; }

	UFUNCTION(BlueprintCallable, Category = "Player Vehicle Gameplay")
	FText GetCurrentSpeed();
	
	UFUNCTION(BlueprintCallable, Category = "Player Vehicle Gameplay")
	void SetCurrentSpeed( const float SpeedIn);
	
	UFUNCTION(BlueprintCallable, Category = "Player Vehicle Gameplay")
	float GetCurrentLife() const { return 1.f - ((StartingLife - CurrentPlayerLife) / StartingLife); }
	
	UFUNCTION(BlueprintCallable, Category = "Player Vehicle Gameplay")
	void SetGameIsPaused(const bool PausedIn) { bGameIsPaused = PausedIn; }

	UFUNCTION(BlueprintCallable, Category = "Player Vehicle Gameplay")
	void SetCanTurn( const bool CanTurnIn) { bCanTurn = CanTurnIn; }

	void SetDesiredRotation(const float NewRotation) { DesiredRotation = NewRotation; }

	void SetNewLocation(bool MoveXIn, bool MoveYIn, float LocationIn);

	void SetNewPositionAfterTurn(const FVector ArrowLocation, const FRotator NewPlayerRotation);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void TimelineFloatReturn(float Value);

	UFUNCTION()
	void OnTimelineFinished();

	/** Declare a delegate to call with TimeLineFloatReturn */
	FOnTimelineFloat InterpFunction{};

	/** Declare a delegate to call with OnTimelineFinished */
	FOnTimelineEvent TimelineFinished{};
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void ChangePlayerLife(const float LifeChangeValue);

	float GetSpeedOfAcceleration() const { return SpeedOfAcceleration; }
private:
	bool bGameIsPaused;
	bool bIsTurning;

	float DesiredRotation;
	bool bCanTurn;

	bool bHasTurned;

	TArray<float> YLocations;
	int32 LocationIndex;
	int32 LastLocationIndex;
	float YLocationToMoveTo;
	float TimeSinceTurnStarted;
	float CurrentSpeed;
	float LocationOffset;

	// Variables for turning player on a curve
	FVector StartPlayerLocation;
	FVector EndPlayerLocation;
	FRotator StartPlayerRotation;
	FRotator EndPlayerRotation;
	
	void AddPlayerWidgetToScreen();
	
	void MoveVehicleForward(float DeltaTime);

	void TurnLeft();
	void TurnRight();
	void Turn(const float DeltaTime);

	float CurrentPlayerLife;

	UPROPERTY()
	UTimelineComponent* TimelineComponent;

	// To quickly flash the player speed, making it look like it's varying slightly
	float TimeSinceSpeedoVaried;
	
	/** Curve to use when turning the vehicle round a corner */
	UPROPERTY()
	class UCurveFloat* FCurve;

	void GetMovementCurve();
	
	// Time taken for curve to complete in seconds
	float MaxCurveTimeRange;

};


