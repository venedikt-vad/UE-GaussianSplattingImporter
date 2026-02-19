// GaussianSplatBPLibrary.cpp
#include "GaussianSplatBPLibrary.h"
#include "PlySplatLoader.h"

UGaussianSplatComponent* UGaussianSplatBPLibrary::LoadSplatsToActor(AActor* TargetActor, const FString& PlyFilePath)
{
	if (!TargetActor) return nullptr;

	TArray<FSplat> Splats;
	FString Error;
	if (!FPlySplatLoader::LoadSplatsFromPly(PlyFilePath, Splats, Error))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load splats: %s"), *Error);
		return nullptr;
	}

	UGaussianSplatComponent* Comp = NewObject<UGaussianSplatComponent>(TargetActor);
	Comp->RegisterComponent();
	Comp->AttachToComponent(TargetActor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	Comp->BuildInstances(Splats);

	return Comp;
}

UGaussianSplatComponent* UGaussianSplatBPLibrary::LoadSplatsToComp(UGaussianSplatComponent* Target, const FString& PlyFilePath)
{
	if (!Target) return nullptr;

	TArray<FSplat> Splats;
	FString Error;
	if (!FPlySplatLoader::LoadSplatsFromPly(PlyFilePath, Splats, Error))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load splats: %s"), *Error);
		return nullptr;
	}

	UGaussianSplatComponent* Comp = Target;
	Comp->BuildInstances(Splats);
	return Comp;
}
