// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ReaperTeleport.generated.h"

UCLASS()
class DEATHMATCH_API AReaperTeleport : public AActor
{
	GENERATED_BODY()
	
public:	
	AReaperTeleport();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	void SetMeshHide(bool bHide);

public:
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* TeleportMesh = nullptr;

	UPROPERTY(EditAnywhere)
	class UCapsuleComponent* Capsule = nullptr;
};
