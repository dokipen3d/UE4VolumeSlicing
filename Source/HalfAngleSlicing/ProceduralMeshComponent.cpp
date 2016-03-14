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
#include "DynamicMeshBuilder.h"
#include "ProceduralMeshComponent.h"
#include "Runtime/Launch/Resources/Version.h"
#include "Kismet/GameplayStatics.h"
#include "Engine.h"

/** Vertex Buffer */
class FProceduralMeshVertexBuffer : public FVertexBuffer
{
public:
	TArray<FDynamicMeshVertex> Vertices;

	virtual void InitRHI() override
	{
		FRHIResourceCreateInfo CreateInfo;
		VertexBufferRHI = RHICreateVertexBuffer(Vertices.Num() * sizeof(FDynamicMeshVertex), BUF_Dynamic, CreateInfo);
		// Copy the vertex data into the vertex buffer.
		void* VertexBufferData = RHILockVertexBuffer(VertexBufferRHI, 0, Vertices.Num() * sizeof(FDynamicMeshVertex), RLM_WriteOnly);
		FMemory::Memcpy(VertexBufferData, Vertices.GetData(), Vertices.Num() * sizeof(FDynamicMeshVertex));
		RHIUnlockVertexBuffer(VertexBufferRHI);
	}
};

/** Index Buffer */
class FProceduralMeshIndexBuffer : public FIndexBuffer
{
public:
	TArray<int32> Indices;

	virtual void InitRHI() override
	{
		FRHIResourceCreateInfo CreateInfo;
		IndexBufferRHI = RHICreateIndexBuffer(sizeof(int32), Indices.Num() * sizeof(int32), BUF_Dynamic, CreateInfo);
		// Write the indices to the index buffer.
		void* Buffer = RHILockIndexBuffer(IndexBufferRHI, 0, Indices.Num() * sizeof(int32), RLM_WriteOnly);
		FMemory::Memcpy(Buffer, Indices.GetData(), Indices.Num() * sizeof(int32));
		RHIUnlockIndexBuffer(IndexBufferRHI);
	}
};

/** Vertex Factory */
class FProceduralMeshVertexFactory : public FLocalVertexFactory
{
public:
	FProceduralMeshVertexFactory()
	{
	}

	/** Initialization */
	void Init(const FProceduralMeshVertexBuffer* VertexBuffer)
	{
		// Commented out to enable building light of a level (but no backing is done for the procedural mesh itself)
		//check(!IsInRenderingThread());

		ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
			InitProceduralMeshVertexFactory,
			FProceduralMeshVertexFactory*, VertexFactory, this,
			const FProceduralMeshVertexBuffer*, VertexBuffer, VertexBuffer,
			{
				// Initialize the vertex factory's stream components.
				DataType NewData;
				NewData.PositionComponent = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FDynamicMeshVertex, Position, VET_Float3);
				NewData.TextureCoordinates.Add(
					FVertexStreamComponent(VertexBuffer, STRUCT_OFFSET(FDynamicMeshVertex, TextureCoordinate), sizeof(FDynamicMeshVertex), VET_Float2)
					);
				NewData.TangentBasisComponents[0] = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FDynamicMeshVertex, TangentX, VET_PackedNormal);
				NewData.TangentBasisComponents[1] = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FDynamicMeshVertex, TangentZ, VET_PackedNormal);
				NewData.ColorComponent = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FDynamicMeshVertex, Color, VET_Color);
				VertexFactory->SetData(NewData);
			});
	}
};



/** Scene proxy */
class FProceduralMeshSceneProxy : public FPrimitiveSceneProxy
{
public:

	FProceduralMeshSceneProxy(UProceduralMeshComponent* Component)
		: FPrimitiveSceneProxy(Component)
		, MaterialRelevance(Component->GetMaterialRelevance(GetScene().GetFeatureLevel()))
	{
		// Add each triangle to the vertex/index buffer
		for (int VertIdx = 0; VertIdx<Component->ProceduralMeshVerts.Num(); VertIdx++)
		{

			FVector& Vert = Component->ProceduralMeshVerts[VertIdx];

		
			const FVector TangentZ = Component->halfVector.GetSafeNormal();
			const FVector TangentX = FVector::CrossProduct(TangentZ, FVector::ForwardVector).GetSafeNormal();
			const FVector TangentY = FVector::CrossProduct(TangentZ, TangentY).GetSafeNormal();




			FDynamicMeshVertex Vert0;

			Vert0.Position = Vert;
			Vert0.Color = FColor((Vert.X+0.5)*255, (Vert.Y+0.5)*255, (Vert.Z+0.5)*255);
			
			Vert0.SetTangents(TangentX, TangentZ, TangentY);

			Vert0.TextureCoordinate.Set(Vert.X, Vert.Y);
			VertexBuffer.Vertices.Add(Vert0);

			// IndexBuffer.Indices.Add(VIndex);

			//            FDynamicMeshVertex Vert1;
			//            Vert1.Position = Tri.Vertex1.Position;
			//            Vert1.Color = Tri.Vertex1.Color;
			//            Vert1.SetTangents(TangentX, TangentY, TangentZ);
			//            Vert1.TextureCoordinate.Set(Tri.Vertex1.U, Tri.Vertex1.V);
			//            VIndex = VertexBuffer.Vertices.Add(Vert1);
			// IndexBuffer.Indices.Add(VIndex);

			//            FDynamicMeshVertex Vert2;
			//            Vert2.Position = Tri.Vertex2.Position;
			//            Vert2.Color = Tri.Vertex2.Color;
			//            Vert2.SetTangents(TangentX, TangentY, TangentZ);
			//            Vert2.TextureCoordinate.Set(Tri.Vertex2.U, Tri.Vertex2.V);
			//            VIndex = VertexBuffer.Vertices.Add(Vert2);
			// IndexBuffer.Indices.Add(VIndex);
		}
		// Add each triangle to the vertex/index buffer
		for (int IndIdx = 0; IndIdx<Component->ProceduralMeshIndices.Num(); IndIdx++)
		{
			IndexBuffer.Indices.Add(Component->ProceduralMeshIndices[IndIdx]);

		}
		//GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Green, TEXT("created buffers"));
		//UE_LOG(LogTemp, Warning, TEXT("created buffers message"));
		// Init vertex factory
		VertexFactory.Init(&VertexBuffer);

		// Enqueue initialization of render resource
		BeginInitResource(&VertexBuffer);
		BeginInitResource(&IndexBuffer);
		BeginInitResource(&VertexFactory);

		// Grab material
		Material = Component->GetMaterial(0);
		if (Material == NULL)
		{
			Material = UMaterial::GetDefaultMaterial(MD_Surface);
		}

	}

	virtual ~FProceduralMeshSceneProxy()
	{
		VertexBuffer.ReleaseResource();
		IndexBuffer.ReleaseResource();
		VertexFactory.ReleaseResource();
	}

	virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_ProceduralMeshSceneProxy_GetDynamicMeshElements);

		const bool bWireframe = AllowDebugViewmodes() && ViewFamily.EngineShowFlags.Wireframe;

		auto WireframeMaterialInstance = new FColoredMaterialRenderProxy(
			GEngine->WireframeMaterial ? GEngine->WireframeMaterial->GetRenderProxy(IsSelected()) : NULL,
			FLinearColor(0, 0.5f, 1.f)
			);

		Collector.RegisterOneFrameMaterialProxy(WireframeMaterialInstance);

		FMaterialRenderProxy* MaterialProxy = NULL;
		if (bWireframe)
		{
			MaterialProxy = WireframeMaterialInstance;
		}
		else
		{
			MaterialProxy = Material->GetRenderProxy(IsSelected());
		}

		for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
		{
			if (VisibilityMap & (1 << ViewIndex))
			{
				const FSceneView* View = Views[ViewIndex];
				// Draw the mesh.
				FMeshBatch& Mesh = Collector.AllocateMesh();
				FMeshBatchElement& BatchElement = Mesh.Elements[0];
				BatchElement.IndexBuffer = &IndexBuffer;
				Mesh.bWireframe = bWireframe;
				Mesh.VertexFactory = &VertexFactory;
				Mesh.MaterialRenderProxy = MaterialProxy;
				BatchElement.PrimitiveUniformBuffer = CreatePrimitiveUniformBufferImmediate(GetLocalToWorld(), GetBounds(), GetLocalBounds(), true, UseEditorDepthTest());
				BatchElement.FirstIndex = 0;
				BatchElement.NumPrimitives = IndexBuffer.Indices.Num() / 3;
				BatchElement.MinVertexIndex = 0;
				BatchElement.MaxVertexIndex = VertexBuffer.Vertices.Num() - 1;
				Mesh.ReverseCulling = IsLocalToWorldDeterminantNegative();
				Mesh.Type = PT_TriangleList;
				Mesh.DepthPriorityGroup = SDPG_World;
				Mesh.bCanApplyViewModeOverrides = false;
				Collector.AddMesh(ViewIndex, Mesh);
			}
		}
	}

	virtual void DrawDynamicElements(FPrimitiveDrawInterface* PDI, const FSceneView* View)
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_ProceduralMeshSceneProxy_DrawDynamicElements);

		const bool bWireframe = AllowDebugViewmodes() && View->Family->EngineShowFlags.Wireframe;

		FColoredMaterialRenderProxy WireframeMaterialInstance(
			GEngine->WireframeMaterial ? GEngine->WireframeMaterial->GetRenderProxy(IsSelected()) : NULL,
			FLinearColor(0, 0.5f, 1.f)
			);

		FMaterialRenderProxy* MaterialProxy = NULL;
		if (bWireframe)
		{
			MaterialProxy = &WireframeMaterialInstance;
		}
		else
		{
			MaterialProxy = Material->GetRenderProxy(IsSelected());
		}

		// Draw the mesh.
		FMeshBatch Mesh;
		FMeshBatchElement& BatchElement = Mesh.Elements[0];
		BatchElement.IndexBuffer = &IndexBuffer;
		Mesh.bWireframe = bWireframe;
		Mesh.VertexFactory = &VertexFactory;
		Mesh.MaterialRenderProxy = MaterialProxy;
		BatchElement.PrimitiveUniformBuffer = CreatePrimitiveUniformBufferImmediate(GetLocalToWorld(), GetBounds(), GetLocalBounds(), true, UseEditorDepthTest());
		BatchElement.FirstIndex = 0;
		BatchElement.NumPrimitives = IndexBuffer.Indices.Num() / 3;
		BatchElement.MinVertexIndex = 0;
		BatchElement.MaxVertexIndex = VertexBuffer.Vertices.Num() - 1;
		Mesh.ReverseCulling = IsLocalToWorldDeterminantNegative();
		Mesh.Type = PT_TriangleList;
		Mesh.DepthPriorityGroup = SDPG_World;
		PDI->DrawMesh(Mesh);
	}

	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View)
	{
		FPrimitiveViewRelevance Result;
		Result.bDrawRelevance = IsShown(View);
		Result.bShadowRelevance = IsShadowCast(View);
		Result.bDynamicRelevance = true;
		MaterialRelevance.SetPrimitiveViewRelevance(Result);
		return Result;
	}

	virtual bool CanBeOccluded() const override
	{
		return !MaterialRelevance.bDisableDepthTest;
	}

	virtual uint32 GetMemoryFootprint(void) const
	{
		return(sizeof(*this) + GetAllocatedSize());
	}

	uint32 GetAllocatedSize(void) const
	{
		return(FPrimitiveSceneProxy::GetAllocatedSize());
	}

private:

	UMaterialInterface* Material;
	FProceduralMeshVertexBuffer VertexBuffer;
	FProceduralMeshIndexBuffer IndexBuffer;
	FProceduralMeshVertexFactory VertexFactory;

	FMaterialRelevance MaterialRelevance;
};


//////////////////////////////////////////////////////////////////////////

UProceduralMeshComponent::UProceduralMeshComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	// bTickInEditor = true;
	//bNeverNeedsRenderUpdate = false;
	//bAutoActivate = true;

	//SetCollisionProfileName(UCollisionProfile::BlockAllDynamic_ProfileName);
	//SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);


	//get the cameramanager for the pos
	PCM = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);

	FVector boxV[] = {
		FVector(-0.5f, -0.5f, 0.5f),    //bottom left front
		FVector(-0.5f, 0.5f, 0.5f),     //top left front
		FVector(0.5f, 0.5f, 0.5f),      //top right front
		FVector(0.5f, -0.5f, 0.5f),     //bottom right front
		FVector(-0.5f, -0.5f, -0.5f),   //bottom left back
		FVector(-0.5f, 0.5f, -0.5f),    //top left back
		FVector(0.5f, 0.5f, -0.5f),     //top right back
		FVector(0.5f, -0.5f, -0.5f)     //bottom right back
	};

	boxVertices.Append(boxV, ARRAY_COUNT(boxV));



	FVector boxD[] = {
		FVector(0.0f, -1.0f, 0.0f),    //down
		FVector(1.0f, 0.0f, 0.0f),    //right
		FVector(0.0f, 0.0f, -1.0f),    // back

		FVector(-1.0f, 0.0f, 0.0f),   //left
		FVector(0.0f, 1.0f, 0.0f),   //up
		FVector(0.0f, 0.0f, -1.0f),   //back

		FVector(0.0f, 0.0f, 1.0f),		//forward
		FVector(0.0f, 1.0f, 0.0f),    //up
		FVector(1.0f, 0.0f, 0.0f),		//right

		FVector(0.0f, 0.0f, 1.0f),		//forward
		FVector(-1.0f, 0.0f, 0.0f),		//left
		FVector(0.0f, -1.0f, 0.0f),		//down

	};

	boxEdgeDirections.Append(boxD, ARRAY_COUNT(boxD));

	int eto[] = {
		0, 2, 5,
		0, 2, 7,
		0, 5, 7,
		2, 5, 7
	};

	edgeTestOrder.Append(eto, ARRAY_COUNT(eto));

	int vto[] = {
		1,
		3,
		4,
		6
	};

	vertexTestOrder.Append(vto, ARRAY_COUNT(vto));
}

bool UProceduralMeshComponent::SetProceduralMeshTriangles(const TArray<FProceduralMeshTriangle>& Triangles)
{
	ProceduralMeshTris = Triangles;

	UpdateCollision();

	// Need to recreate scene proxy to send it over
	MarkRenderStateDirty();

	return true;
}

void UProceduralMeshComponent::AddProceduralMeshTriangles(const TArray<FProceduralMeshTriangle>& Triangles)
{
	ProceduralMeshTris.Append(Triangles);

	// Need to recreate scene proxy to send it over
	MarkRenderStateDirty();
}

void  UProceduralMeshComponent::ClearProceduralMeshTriangles()
{
	ProceduralMeshTris.Reset();

	// Need to recreate scene proxy to send it over
	MarkRenderStateDirty();
}


FPrimitiveSceneProxy* UProceduralMeshComponent::CreateSceneProxy()
{
	FPrimitiveSceneProxy* Proxy = NULL;
	// Only if have enough triangles
	if (ProceduralMeshVerts.Num() > 0)
	{
		Proxy = new FProceduralMeshSceneProxy(this);
		//GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Red, TEXT("madeProxy"));

	}
	return Proxy;
}

int32 UProceduralMeshComponent::GetNumMaterials() const
{
	return 1;
}


FBoxSphereBounds UProceduralMeshComponent::CalcBounds(const FTransform & LocalToWorld) const
{
	// Only if have enough triangles
	if (ProceduralMeshVerts.Num() > 0)
	{
		
	    FVector nv2 = FVector(0.5, 0.5, 0.5);
	


		return FBoxSphereBounds(FVector(0, 0, 0), nv2, nv2.Size()).TransformBy(LocalToWorld);

	
	}
	else
	{
		return FBoxSphereBounds();
	}
}


//bool UProceduralMeshComponent::GetPhysicsTriMeshData(struct FTriMeshCollisionData* CollisionData, bool InUseAllTriData)
//{
//    FTriIndices Triangle;
//    
//    for(int32 i = 0; i<ProceduralMeshTris.Num(); i++)
//    {
//        const FProceduralMeshTriangle& tri = ProceduralMeshTris[i];
//        
//        Triangle.v0 = CollisionData->Vertices.Add(tri.Vertex0.Position);
//        Triangle.v1 = CollisionData->Vertices.Add(tri.Vertex1.Position);
//        Triangle.v2 = CollisionData->Vertices.Add(tri.Vertex2.Position);
//        
//        CollisionData->Indices.Add(Triangle);
//        CollisionData->MaterialIndices.Add(i);
//    }
//    
//    CollisionData->bFlipNormals = true;
//    
//    return true;
//}

//bool UProceduralMeshComponent::ContainsPhysicsTriMeshData(bool InUseAllTriData) const
//{
//    return (ProceduralMeshTris.Num() > 0);
//}

void UProceduralMeshComponent::UpdateBodySetup()
{
	if (ModelBodySetup == NULL)
	{
		ModelBodySetup = ConstructObject<UBodySetup>(UBodySetup::StaticClass(), this);
		ModelBodySetup->CollisionTraceFlag = CTF_UseComplexAsSimple;
		ModelBodySetup->bMeshCollideAll = false;
	}
}

void UProceduralMeshComponent::UpdateCollision()
{
	if (bPhysicsStateCreated)
	{
		DestroyPhysicsState();
		UpdateBodySetup();
		CreatePhysicsState();

		// Works in Packaged build only since UE4.5:
		ModelBodySetup->InvalidatePhysicsData();
		ModelBodySetup->CreatePhysicsMeshes();
	}
}

UBodySetup* UProceduralMeshComponent::GetBodySetup()
{
	UpdateBodySetup();
	return ModelBodySetup;
}

void UProceduralMeshComponent::UpdateVectors()
{
	//get the player 0 camera position (single player)
	//camPosition = PCM->GetCameraLocation();
	camPosition = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetCameraLocation();

	//camPosition = FVector(4,5,6);
	//GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Red, TEXT("got cam"));

	//get the inverse matrix of this scene component
	objectMatrix = ComponentToWorld.ToInverseMatrixWithScale();

	inverseCamPos = objectMatrix.TransformPosition(camPosition);
	invCamVector = -inverseCamPos;
	invCamVector.Normalize();

	if (Dlight)
	{
		invLightVector = objectMatrix.TransformVector(Dlight->GetActorForwardVector());
	}
	else{
		invLightVector = FVector(1, 1, 1);

	}

	//    UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject);
	//    if(!World) return false;
	//    //~~~~~~~~~~~
	//    
	//    return (World->WorldType == EWorldType::Editor )


	invLightVector.Normalize();

	float DotProductcamLight = FVector::DotProduct(invLightVector, invCamVector);
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("dotP is %f"),DotProductcamLight));

	bViewInverted = DotProductcamLight >= 0;

	halfVector = ((bViewInverted ? invCamVector : invCamVector * -1.0f) + invLightVector);
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, halfVector.ToString());


	//halfVector = FVector(1.0, 1.0, 1.0);
	halfVector.Normalize();
}

//calc ray intersection
bool UProceduralMeshComponent::calcRayPlaneItersection(FVector boxEdgeOrigin, FVector boxEdgeDirection, FVector planeOrigin, FVector planeNormal, float &intersectionDistanceAlongEdge)
{
	float denominator = FVector::DotProduct(planeNormal, boxEdgeDirection);
	if (denominator != 0.0f){
		FVector boxEdgeOriginMinusPlaneOrigin = planeOrigin - boxEdgeOrigin;
		intersectionDistanceAlongEdge = FVector::DotProduct(boxEdgeOriginMinusPlaneOrigin, planeNormal) / denominator;
		return ((intersectionDistanceAlongEdge >= 0) && (intersectionDistanceAlongEdge <= 1));
		//return true;

	}//
	return false;
}

void UProceduralMeshComponent::refillVertexArray()
{
	ProceduralMeshVerts.Reset();
	ProceduralMeshIndices.Reset();

	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("component ticking"));
	//reset vert count
	totalVerts = 0;



	

	//loop through vertices and get min and max distances (in magnitude) from 0,0,0 based on incoming vector
	max_dist = FVector::DotProduct(halfVector, boxVertices[0]);
	min_dist = max_dist;
	int max_index = 0;
	int count = 0;
	for (int i = 1; i<8; i++) {
		float dist = FVector::DotProduct(halfVector, boxVertices[i]);
		if (dist > max_dist) {
			max_dist = dist;
			max_index = i;
		}
		if (dist<min_dist)
			min_dist = dist;
	}


	FVector newpos = ((halfVector*min_dist));
	FVector newdir = ((halfVector*max_dist));
	float halfVectorLength = (newpos - newdir).Size();
	float stepSize = halfVectorLength / (float)numSlices;

	FVector halfVectorNorm = halfVector;
	halfVectorNorm.Normalize();
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("vectorlength is %f"), halfVectorLength));

	for (int s = 0; s < numSlices; s++){

		TArray<FVector> localVertexList;
		TArray<int32> localIndicesList;
		//TArray<FVector2D>localuvList;


		int numVerticesThisSlice = 0;


		FVector C = newpos + (halfVectorNorm*s*stepSize) + (((halfVectorNorm*stepSize) / 2.0f));


		for (int e = 0; e < 12; e++){

			int edgeTestIndex = vertexTestOrder[(int)((e / (int)3))];
			//set start point
			FVector boxPoint = boxVertices[edgeTestIndex];
			//set dir
			FVector dir = boxEdgeDirections[e];

			float t = 0;

			if (calcRayPlaneItersection(
				boxPoint,
				dir,
				C,
				halfVector,
				t)){

				numVerticesThisSlice++;
				localVertexList.Add(boxPoint + (dir*t));
				//localuvList.Add(new Vector2(0.0f, 0.0f));
			}

		}

		localVertexList.Sort([&](const FVector A, const FVector B) {
			if (FVector::DotProduct(halfVector, FVector::CrossProduct(A - C, B - C)) <= 0){
				return true;
			}
			else{
				return false;
			}
		});//end of sort


		//				There's no need to convert everything to 2D.

		//You have the center C and the normal n. To determine whether point B is clockwise or counterclockwise from point A,
		//calculate dot(n, cross(A-C, B-C)). If the result is positive, B is counterclockwise from A; if it's negative, B is clockwise from A.
		//add verts and indices to list
		//int vert = 1;
		FVector centroid = FVector(0.0f, 0.0f, 0.0f);


		//localuvList.Add(new Vector2(0.0f, 0.0f));
		for (int p = 0; p < numVerticesThisSlice; p++){
			localIndicesList.Add(totalVerts);
			if (bViewInverted)
			{
				localIndicesList.Add(((p + 2) % numVerticesThisSlice + 1) + totalVerts);
				localIndicesList.Add(((p + 1) % numVerticesThisSlice + 1) + totalVerts);
			}
			else
			{
				localIndicesList.Add(((p + 1) % numVerticesThisSlice + 1) + totalVerts);
				localIndicesList.Add(((p + 2) % numVerticesThisSlice + 1) + totalVerts);
			}
			//            if (bViewInverted){
			//                GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("true"));
			//                
			//            }
			//            else
			//            {
			//                GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("false"));
			//                
			//            }


			centroid += localVertexList[p];
		}

		centroid /= numVerticesThisSlice;
		localVertexList.Insert(centroid, 0);

		ProceduralMeshVerts.Append(localVertexList);
		if (!bViewInverted){
			ProceduralMeshIndices.Append(localIndicesList);
		}
		else{
			ProceduralMeshIndices.Insert(localIndicesList,0);
		}
		
		// uvs.AddRange(localuvList);
		totalVerts += numVerticesThisSlice + 1;

	}
	//GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Red, FString::Printf(TEXT("numverts%i"),totalVerts));

}

void UProceduralMeshComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	//GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Red, TEXT("component tick"));



	//reset all needed transforms/vectors
	UpdateVectors();
	refillVertexArray();
	//do an initial clear
	//UpdateCollision();
	// Need to recreate scene proxy to send it over
	MarkRenderStateDirty();


}


