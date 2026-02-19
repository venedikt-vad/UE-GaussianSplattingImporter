#pragma once
#include "CoreMinimal.h"

struct FSplat
{
	FVector Position = FVector::ZeroVector;

	// Gaussian Splatting typically stores scale + rotation (quat) to define 3D covariance.
	FVector Scale = FVector(1,1,1);
	FQuat   Rotation = FQuat::Identity;

	// DC color (or RGB) + opacity
	FLinearColor Color = FLinearColor::White;
	float Opacity = 1.0f;
};