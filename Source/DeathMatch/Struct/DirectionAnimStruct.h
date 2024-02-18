#pragma once

#include "CoreMinimal.h"
#include "DirectionAnimStruct.generated.h"

USTRUCT(BlueprintType)
struct FDirectionAnimations
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimSequence* Foward = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimSequence* Backward = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimSequence* Right = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimSequence* Left = nullptr;
};

class DEATHMATCH_API DirectionAnimStruct
{
public:
	DirectionAnimStruct();
	~DirectionAnimStruct();
};
