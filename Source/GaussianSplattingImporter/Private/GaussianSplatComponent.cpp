#include "GaussianSplatComponent.h"

UGaussianSplatComponent::UGaussianSplatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	HISM = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("SplatHISM"));
	HISM->SetupAttachment(this);

	HISM->SetMobility(EComponentMobility::Movable);
	HISM->NumCustomDataFloats = CustomFloatCount;

	// IMPORTANT: Assign a quad mesh + a splat material in editor or in code.
	// HISM->SetStaticMesh(...);
	// HISM->SetMaterial(0, ...);
}

void UGaussianSplatComponent::BuildInstances(const TArray<FSplat>& Splats)
{
	HISM->ClearInstances();

	// Ensure custom float count set before adding instances
	HISM->NumCustomDataFloats = CustomFloatCount;

	for (int32 i = 0; i < Splats.Num(); ++i)
	{
		const FSplat& S = Splats[i];

		// A unit quad instance at position; orientation can be identity if shader billboard-orients.
		const FTransform Xf(FQuat::Identity, S.Position, FVector(1,1,1));
		const int32 InstanceIndex = HISM->AddInstance(Xf);

		// Pack: [0..2]=scale, [3..6]=quat, [7..9]=color, [10]=opacity, [11]=unused
		HISM->SetCustomDataValue(InstanceIndex, 0, S.Scale.X, false);
		HISM->SetCustomDataValue(InstanceIndex, 1, S.Scale.Y, false);
		HISM->SetCustomDataValue(InstanceIndex, 2, S.Scale.Z, false);

		HISM->SetCustomDataValue(InstanceIndex, 3, S.Rotation.X, false);
		HISM->SetCustomDataValue(InstanceIndex, 4, S.Rotation.Y, false);
		HISM->SetCustomDataValue(InstanceIndex, 5, S.Rotation.Z, false);
		HISM->SetCustomDataValue(InstanceIndex, 6, S.Rotation.W, false);

		HISM->SetCustomDataValue(InstanceIndex, 7, S.Color.R, false);
		HISM->SetCustomDataValue(InstanceIndex, 8, S.Color.G, false);
		HISM->SetCustomDataValue(InstanceIndex, 9, S.Color.B, false);

		HISM->SetCustomDataValue(InstanceIndex, 10, S.Opacity, false);
		HISM->SetCustomDataValue(InstanceIndex, 11, 0.0f, false);
	}

	// Push updates once at the end
	HISM->MarkRenderStateDirty();
}
