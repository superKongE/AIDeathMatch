// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DeathMatch/Character/RootCharacter.h"
#include "GideonCharacter.generated.h"

/**
 * 
 */
UCLASS()
class DEATHMATCH_API AGideonCharacter : public ARootCharacter
{
	GENERATED_BODY()
	
public:
	AGideonCharacter();

	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

	virtual void Jump() override;

private:
	class UGideonCombatComponent* GideonCombatComponent = nullptr;
};
