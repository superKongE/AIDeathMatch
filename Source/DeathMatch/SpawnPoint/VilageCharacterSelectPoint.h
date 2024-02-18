// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VilageCharacterSelectPoint.generated.h"

UCLASS()
class DEATHMATCH_API AVilageCharacterSelectPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	AVilageCharacterSelectPoint();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

public:

private:
	UPROPERTY()
	class USkeletalMeshComponent* CharactetMesh = nullptr;


};
