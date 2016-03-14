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

#pragma once

#include "Components/MeshComponent.h"
#include "ProceduralMeshComponent.generated.h"

USTRUCT(BlueprintType)
struct FProceduralMeshVertex
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere, Category = Triangle)
		FVector Position;

	UPROPERTY(EditAnywhere, Category = Triangle)
		FColor Color;

	UPROPERTY(EditAnywhere, Category = Triangle)
		float U;

	UPROPERTY(EditAnywhere, Category = Triangle)
		float V;
};

USTRUCT(BlueprintType)
struct FProceduralMeshTriangle
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere, Category = Triangle)
		FProceduralMeshVertex Vertex0;

	UPROPERTY(EditAnywhere, Category = Triangle)
		FProceduralMeshVertex Vertex1;

	UPROPERTY(EditAnywhere, Category = Triangle)
		FProceduralMeshVertex Vertex2;
};

/** Component that allows you to specify custom triangle mesh geometry */
//UCLASS(editinlinenew, meta = (BlueprintSpawnableComponent), ClassGroup=Rendering)
//class UProceduralMeshComponent : public UMeshComponent, public IInterface_CollisionDataProvider

UCLASS(editinlinenew, meta = (BlueprintSpawnableComponent), ClassGroup = Rendering)
class UProceduralMeshComponent : public UMeshComponent
{
	GENERATED_UCLASS_BODY()

public:

	class APlayerCameraManager* PCM;

	UPROPERTY(EditAnywhere, Category = "Volume")
		ADirectionalLight* Dlight;


	FMatrix objectMatrix;
	FMatrix inverseMatrix;
	FVector inverseCamPos;
	FVector camPosition;
	FVector invCamVector;
	FVector invLightVector;
	FVector halfVector;
	bool bViewInverted;
	float min_dist;
	float max_dist;

	int totalVerts = 0;
	TArray<FVector> boxVertices;
	TArray<FVector> boxEdgeDirections;
	TArray<int> edgeTestOrder;
	TArray<int> vertexTestOrder;

	UPROPERTY(EditAnywhere, Category = "Volume")
		int32 numSlices = 32;

	void refillVertexArray();
	bool calcRayPlaneItersection(FVector boxEdgeOrigin, FVector boxEdgeDirection, FVector planeOrigin, FVector planeNormal, float &intersectionDistanceAlongEdge);


	//update inverseCampos for vertex generation in object space
	void UpdateVectors();

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	/** Set the geometry to use on this triangle mesh */
	UFUNCTION(BlueprintCallable, Category = "Components|ProceduralMesh")
		bool SetProceduralMeshTriangles(const TArray<FProceduralMeshTriangle>& Triangles);


	/** Add to the geometry to use on this triangle mesh.  This may cause an allocation.  Use SetCustomMeshTriangles() instead when possible to reduce allocations. */
	UFUNCTION(BlueprintCallable, Category = "Components|ProceduralMesh")
		void AddProceduralMeshTriangles(const TArray<FProceduralMeshTriangle>& Triangles);

	/** Removes all geometry from this triangle mesh.  Does not deallocate memory, allowing new geometry to reuse the existing allocation. */
	UFUNCTION(BlueprintCallable, Category = "Components|ProceduralMesh")
		void ClearProceduralMeshTriangles();

	/** Description of collision */
	UPROPERTY(BlueprintReadOnly, Category = "Collision")
	class UBodySetup* ModelBodySetup;

	// Begin Interface_CollisionDataProvider Interface
	//    virtual bool GetPhysicsTriMeshData(struct FTriMeshCollisionData* CollisionData, bool InUseAllTriData) override;
	//    virtual bool ContainsPhysicsTriMeshData(bool InUseAllTriData) const override;
	//    virtual bool WantsNegXTriMesh() override{ return false; }
	// End Interface_CollisionDataProvider Interface

	// Begin UPrimitiveComponent interface.
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual class UBodySetup* GetBodySetup() override;
	// End UPrimitiveComponent interface.

	// Begin UMeshComponent interface.
	virtual int32 GetNumMaterials() const override;
	// End UMeshComponent interface.

	void UpdateBodySetup();
	void UpdateCollision();


private:
	// Begin USceneComponent interface.
	virtual FBoxSphereBounds CalcBounds(const FTransform & LocalToWorld) const override;
	// Begin USceneComponent interface.

	/** */
	TArray<FProceduralMeshTriangle> ProceduralMeshTris;
	TArray<FVector> ProceduralMeshVerts;

	TArray<int32> ProceduralMeshIndices;


	friend class FProceduralMeshSceneProxy;
};
