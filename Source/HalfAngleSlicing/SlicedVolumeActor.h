// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "HalfAngleSlicing.h"
#include "ProceduralMeshComponent.h"

#include "SlicedVolumeActor.generated.h"

UCLASS()
class HALFANGLESLICING_API ASlicedVolumeActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASlicedVolumeActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

    UPROPERTY(VisibleAnywhere, Category = "Volume")
    UProceduralMeshComponent* volumeComp;
    
    //USceneComponent* tx;
    
    TArray<FProceduralMeshTriangle> newTris;

  	
};
