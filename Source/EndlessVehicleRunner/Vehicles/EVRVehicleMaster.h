// Copyright 2021 DME Games

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/TimelineComponent.h"
#include "EVRVehicleMaster.generated.h"

UCLASS()
class ENDLESSVEHICLERUNNER_API AEVRVehicleMaster : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AEVRVehicleMaster();
	
	UPROPERTY(Category = "Game Vehicle", EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	USceneComponent* SceneComponent;
	
	UPROPERTY(Category = "Game Vehicle", EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMeshComp;

	UPROPERTY(Category = "Game Vehicle", EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	class UEVRMovementComponent* MovementComponent;

	// Distance between lanes
	UPROPERTY(EditDefaultsOnly, Category = "Vehicle Options")
	float DistanceBetweenLanes;
	
	// Units in cm the vehicle will move forward per second
	UPROPERTY(EditDefaultsOnly, Category = "Vehicle Options")
	float MovementSpeed;

	// Time in seconds it takes for the vehicle to reach top speed
	UPROPERTY(EditDefaultsOnly, Category = "Vehicle Options")
	float SpeedOfAcceleration;
	
	// Time in seconds it takes to move the vehicle to another lane 
	UPROPERTY(EditDefaultsOnly, Category = "Vehicle Options")
	float TurnSpeed;

	// Amount of visible "turn" in the vehicle when changing lanes
	UPROPERTY(EditDefaultsOnly, Category = "Vehicle Options")
	float TurnDegrees;
	
	UPROPERTY()
	UTimelineComponent* TurnTimeline;

	UFUNCTION(BlueprintCallable, Category = "Vehicle Gameplay")
	float GetMaxSpeed() const { return MovementSpeed; }

	UFUNCTION(BlueprintCallable, Category = "Vehicle Gameplay")
	float GetCurrentSpeedAsFloat() const { return CurrentSpeed; }
	
	UFUNCTION(BlueprintCallable, Category = "Vehicle Gameplay")
	void SetGameIsPaused(const bool PausedIn) { bGameIsPaused = PausedIn; }

	UFUNCTION(BlueprintCallable, Category = "Vehicle Gameplay")
	void SetCanTurn( const bool CanTurnIn) { bCanTurn = CanTurnIn; }

	void SetDesiredRotation(const float NewRotation) { DesiredRotation = NewRotation; }

	void SetNewPositionAfterTurn(const FVector ArrowLocation, const FRotator NewVehicleRotation);

	float GetDistanceBetweenLanes() const { return DistanceBetweenLanes; }
	void AddToLaneLocations(const float LocationIn);

	UFUNCTION()
	virtual void OnMeshBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void TimelineFloatReturn(float Value);

	/** Declare a delegate to call with TimeLineFloatReturn */
	FOnTimelineFloat InterpFunction{};

	// Current speed of the vehicle, when moving
	float CurrentSpeed;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void TurnLeft();
	void TurnRight();
	virtual void Turn(const float DeltaTime);

	bool bGameIsPaused;
	bool bIsTurning;

	TArray<float> YLocations;
	float DesiredRotation;
	bool bCanTurn;
	bool bHasTurned;
	
	int32 LocationIndex;
	int32 LastLocationIndex;
	float YLocationToMoveTo;
	float TimeSinceTurnStarted;

	// Variables for turning player on a curve
	FVector StartVehicleLocation;
	FVector EndVehicleLocation;
	FRotator StartVehicleRotation;
	FRotator EndVehicleRotation;
		
	void MoveVehicleForward(float DeltaTime);
	
	UPROPERTY()
	UTimelineComponent* TimelineComponent;

	/** Curve to use when turning the vehicle round a corner */
	UPROPERTY()
	class UCurveFloat* FCurve;

	void GetMovementCurve();
	
	// Time taken for curve to complete in seconds
	float MaxCurveTimeRange;
};
