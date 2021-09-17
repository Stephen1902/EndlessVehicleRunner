// Copyright 2021 DME Games


#include "EVRRoadsideSpawns.h"

#include "Components/SpotLightComponent.h"

// Sets default values
AEVRRoadsideSpawns::AEVRRoadsideSpawns()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	SetRootComponent(SceneComp);

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	StaticMeshComp->SetupAttachment(SceneComp);
/**
	RoadsideLight1 = CreateDefaultSubobject<USpotLightComponent>("Roadside Light 1");
	RoadsideLight1->SetupAttachment(StaticMeshComp);
	RoadsideLight1->SetRelativeLocation(FVector(-510.f, 960.f, 740.f));
	RoadsideLight1->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
	RoadsideLight1->Intensity = 2500.f;
	RoadsideLight1->LightColor = FColor(241, 239, 215);
	RoadsideLight1->AttenuationRadius = 500.f;

	RoadsideLight2 = CreateDefaultSubobject<USpotLightComponent>("Roadside Light 2");
	RoadsideLight2->SetupAttachment(StaticMeshComp);
	RoadsideLight2->SetRelativeLocation(FVector(510.f, -960.f, 740.f));
	RoadsideLight2->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
	RoadsideLight2->Intensity = 2500.f;
	RoadsideLight2->LightColor = FColor(241, 239, 215);
	RoadsideLight2->AttenuationRadius = 500.f;

	RoadsideLight3 = CreateDefaultSubobject<USpotLightComponent>("Roadside Light 3");
	RoadsideLight3->SetupAttachment(StaticMeshComp);
	RoadsideLight3->SetRelativeLocation(FVector(-510.f, -960.f, 740.f));
	RoadsideLight3->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
	RoadsideLight3->Intensity = 2500.f;
	RoadsideLight3->LightColor = FColor(241, 239, 215);
	RoadsideLight3->AttenuationRadius = 500.f;

	RoadsideLight4 = CreateDefaultSubobject<USpotLightComponent>("Roadside Light 4");
	RoadsideLight4->SetupAttachment(StaticMeshComp);
	RoadsideLight4->SetRelativeLocation(FVector(510.f, 960.f, 740.f));
	RoadsideLight4->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
	RoadsideLight4->Intensity = 2500.f;
	RoadsideLight4->LightColor = FColor(241, 239, 215);
	RoadsideLight4->AttenuationRadius = 500.f;
*/
	static ConstructorHelpers::FObjectFinder<UMaterial> WindowMat (TEXT("Material'/Game/Materials/M_Window.M_Window'"));
	if (WindowMat.Succeeded())
	{
		LightsOffMaterial = WindowMat.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> StreetMat (TEXT("Material'/Game/Materials/M_StreetLight.M_StreetLight'"));
	if (StreetMat.Succeeded())
	{
		LightsOnMaterial = StreetMat.Object;
	}
	
	bIsParkPiece = false;
}

// Called when the game starts or when spawned
void AEVRRoadsideSpawns::BeginPlay()
{
	Super::BeginPlay();

	LightUpWindows();
}

void AEVRRoadsideSpawns::LightUpWindows()
{
	if (StartWindowMat > 0 && EndWindowMat > 0 && LightsOffMaterial && LightsOnMaterial)
	{
		// Loop through the available window materials
		for (int32 i = StartWindowMat; i <= EndWindowMat; ++i)
		{
			
			// Generate chance that window material gets changed
			if (FMath::RandRange(0, 99) <= 14)
			{
				StaticMeshComp->SetMaterial(i, LightsOnMaterial);
				/**
				// Get current material on this window 
				UMaterialInterface* CurrentMaterial = StaticMeshComp->GetMaterial(i);
				// Create Dynamic material from it
				UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(CurrentMaterial, this);
				// Set Dynamic Material to the mesh
				StaticMeshComp->SetMaterial(i, DynMaterial);
				// Set parameter values
				DynMaterial->SetVectorParameterValue(FName("Colour"), FColor(0.985f, 0.889f, 0.118f));
				DynMaterial->SetScalarParameterValue(FName("Emission"), 2.0f);
				*/
			}
			
		}
	}
}

