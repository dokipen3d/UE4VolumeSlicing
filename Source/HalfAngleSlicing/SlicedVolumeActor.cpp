The MIT License (MIT)

Copyright (c) [2015] [Daniel Elliott]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

#include "HalfAngleSlicing.h"
#include "SlicedVolumeActor.h"
#include "ProceduralMeshComponent.h"
#include <Engine.h>



// Sets default values
ASlicedVolumeActor::ASlicedVolumeActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    volumeComp = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("VolumeComponent"));
   // tx = CreateAbstractDefaultSubobject<USceneComponent>("mainTX");
    
//    for (int i = 0; i > -50; i--){
//    
//    FProceduralMeshVertex v0;
//    FProceduralMeshVertex v1;
//    FProceduralMeshVertex v2;
//    
//    FProceduralMeshTriangle t1;
//    
//    v0.Position = FVector(i*30, 0.0f, 0.0f);
//    v0.U=0.0;
//    v0.V=0.0;
//
//    v1.Position = FVector(i*30, 1000.0f, 1000.0f);
//    v1.U=1.0;
//    v1.V=1.0;
//    
//    v2.Position = FVector(i*30, 0.0f, 1000.0f);
//    v2.U=0.0;
//    v2.V=1.0;
//
//    
//    t1.Vertex0 = v0;
//    t1.Vertex1 = v1;
//    t1.Vertex2 = v2;
//    
//    
//    
//    newTris.Add(t1);
//    }

    //volumeComp->SetProceduralMeshTriangles(newTris);
    RootComponent = volumeComp;
  //  volumeComp->AttachTo(tx);
    


}

// Called when the game starts or when spawned
void ASlicedVolumeActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASlicedVolumeActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	FRotator ar = GetActorRotation();
	SetActorRotation(FRotator(ar.Pitch, ar.Yaw + (DeltaTime * 3), ar.Roll));
	//GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Red, FString::Printf(TEXT("actor rotation is %f"), ar.Yaw) );

    //volumeComp->ClearProceduralMeshTriangles();
    //volumeComp->SetProceduralMeshTriangles(newTris);



}

