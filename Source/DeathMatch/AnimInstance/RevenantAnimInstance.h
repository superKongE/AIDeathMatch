// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DeathMatch/AnimInstance/RootAnimInstance.h"
#include "RevenantAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class DEATHMATCH_API URevenantAnimInstance : public URootAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

private:
	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	class ARevenantCharacter* OwnerCharacter = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	class URevenantCombatComponent* OwnerCombatComponent = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	bool IsOffUpperarmTransformModify;

	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	FRotator Upperarm_rRoatator;
};
