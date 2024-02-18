// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DeathMatch/Character/RootCharacter.h"
#include "RevenantCharacter.generated.h"

/**
 * 
 */
UCLASS()
class DEATHMATCH_API ARevenantCharacter : public ARootCharacter
{
	GENERATED_BODY()

public:
	ARevenantCharacter();

	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	void Reload();

	UFUNCTION(BlueprintCallable)
	void ThirdSkillForLoopEnd();

private:
	UPROPERTY(EditAnywhere)
	class UChildActorComponent* BombPoint = nullptr;

	UPROPERTY(EditAnywhere);
	class URevenantCombatComponent* RevenantCombatComponent = nullptr;

public:
	FORCEINLINE UChildActorComponent* GetBombSpawnPoint() { return BombPoint; }
	FORCEINLINE URevenantCombatComponent* GetRevenantCombatComponent() { return RevenantCombatComponent; }
};
