#pragma once
#include "CoreMinimal.h"
#include "SplatData.h"

class FPlySplatLoader
{
public:
	static bool LoadSplatsFromPly(const FString& FilePath, TArray<FSplat>& OutSplats, FString& OutError);
};
