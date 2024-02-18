// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DeathMatch/HUD/RootHUD.h"
#include "GideonHUD.generated.h"

/**
 * 
 */
UCLASS()
class DEATHMATCH_API AGideonHUD : public ARootHUD
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
};
