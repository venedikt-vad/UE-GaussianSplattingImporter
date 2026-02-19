// GaussianSplatComponent.h
#pragma once
#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "SplatData.h"
#include "GaussianSplatComponent.generated.h"

UCLASS(ClassGroup = (Rendering), meta = (BlueprintSpawnableComponent))
class UGaussianSplatComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UGaussianSplatComponent();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UHierarchicalInstancedStaticMeshComponent* HISM;

	// Call after you load splats
	void BuildInstances(const TArray<FSplat>& Splats);

	// How many floats per instance you plan to feed the material
	static constexpr int32 CustomFloatCount = 12; // scale3 + quat4 + color3 + opacity1 = 11 (pad to 12)
};
