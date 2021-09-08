// Copyright 2021 DME Games

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EVRRoadsideSpawns.generated.h"

UCLASS()
class ENDLESSVEHICLERUNNER_API AEVRRoadsideSpawns : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEVRRoadsideSpawns();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Roadside Spawn")
	USceneComponent* SceneComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Roadside Spawn")
	UStaticMeshComponent* StaticMeshComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Roadside Spawn")
	class USpotLightComponent* RoadsideLight1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Roadside Spawn")
	class USpotLightComponent* RoadsideLight2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Roadside Spawn")
	class USpotLightComponent* RoadsideLight3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Roadside Spawn")
	class USpotLightComponent* RoadsideLight4;

	UFUNCTION(BlueprintCallable, Category = "Roadside Spawn")
	bool GetIsParkPiece() const { return bIsParkPiece; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintGetter="GetIsParkPiece", Category = "Roadside Spawn")
	bool bIsParkPiece;
	
	
};
