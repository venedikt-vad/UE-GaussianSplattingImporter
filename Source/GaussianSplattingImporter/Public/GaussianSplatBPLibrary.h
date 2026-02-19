// GaussianSplatBPLibrary.h
#pragma once
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GaussianSplatComponent.h"
#include "GaussianSplatBPLibrary.generated.h"

UCLASS()
class UGaussianSplatBPLibrary : public UBlueprintFunctionLibrary {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="GaussianSplats")
	static UGaussianSplatComponent* LoadSplatsToActor(AActor* TargetActor, const FString& PlyFilePath);

	UFUNCTION(BlueprintCallable, Category = "GaussianSplats")
	static UGaussianSplatComponent* LoadSplatsToComp(UGaussianSplatComponent* Target, const FString& PlyFilePath);
};
