// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HipFacingEnum.generated.h"

UENUM(BlueprintType)
enum class EHipFacing : uint8
{
	EHF_F UMETA(DisplayName = "Foward"),
	EHF_B UMETA(DisplayName = "Backward")
};

class DEATHMATCH_API HipFacingEnum
{
public:
	HipFacingEnum();
	~HipFacingEnum();
};
