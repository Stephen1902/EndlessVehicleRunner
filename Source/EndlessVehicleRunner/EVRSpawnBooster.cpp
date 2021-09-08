// Copyright 2021 DME Games


#include "EVRSpawnBooster.h"
#include "UObject/ConstructorHelpers.h"

AEVRSpawnBooster::AEVRSpawnBooster()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UMaterial> FlatMat(TEXT("Material'/Game/Materials/M_FlatColour.M_FlatColour'"));
	if (FlatMat.Succeeded())
	{
		FlatMaterial = FlatMat.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UMaterial> PulseMat(TEXT("Material'/Game/Materials/M_Pulse.M_Pulse'"));
	if (PulseMat.Succeeded())
	{
		PulseMaterial = PulseMat.Object;
	}

	MeshColour = FLinearColor(0.8f, 0.f, 0.f);
	PulseFrequency = 0.5f;
	PulseEmission = 2.0f;
	bRotates = true;
	RotateSpeed = 180.f;
}

void AEVRSpawnBooster::BeginPlay()
{
	Super::BeginPlay();

	UMaterialInstanceDynamic* DynamicFlatMat = UMaterialInstanceDynamic::Create(FlatMaterial, this);
	DynamicFlatMat->SetVectorParameterValue("Colour", MeshColour);
	StaticMeshComp->SetMaterial(0, DynamicFlatMat);

	UMaterialInstanceDynamic* DynamicPulseMat = UMaterialInstanceDynamic::Create(PulseMaterial, this);
	DynamicPulseMat->SetVectorParameterValue("Colour", MeshColour);
	DynamicPulseMat->SetScalarParameterValue("Frequency", PulseFrequency);
	DynamicPulseMat->SetScalarParameterValue("Emission", PulseEmission);
	StaticMeshComp->SetMaterial(1, DynamicPulseMat);
}

void AEVRSpawnBooster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bRotates)
	{
		FRotator CurrentRotation = GetActorRotation();
		SetActorRotation(FRotator(CurrentRotation.Pitch, CurrentRotation.Yaw + (RotateSpeed * DeltaTime), CurrentRotation.Roll));
		
	}
}
