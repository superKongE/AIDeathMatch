// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DeathMatch/SmartObject/SmartObject.h"
#include "PatrolPath.generated.h"

/**
 * 
 */
UCLASS()
class DEATHMATCH_API APatrolPath : public ASmartObject
{
	GENERATED_BODY()

public:
	APatrolPath();

	virtual void BeginPlay() override;
	
public:
	// AI의 이동 경로 지정
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SplinePath)
	class USplineComponent* Path = nullptr;

	void GetSplinePoints();

	TArray<FVector> Locations;
};
