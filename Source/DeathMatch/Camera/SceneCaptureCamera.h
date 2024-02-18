// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/SceneCapture2D.h"
#include "SceneCaptureCamera.generated.h"

/**
 * 
 */
UCLASS()
class DEATHMATCH_API ASceneCaptureCamera : public ASceneCapture2D
{
	GENERATED_BODY()

public:
	void ActiveCapture(bool bActive);
};
