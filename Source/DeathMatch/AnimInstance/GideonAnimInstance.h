// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DeathMatch/AnimInstance/RootAnimInstance.h"
#include "GideonAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class DEATHMATCH_API UGideonAnimInstance : public URootAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;
	
private:
	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	class AGideonCharacter* OwnerCharacter = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	class UGideonCombatComponent* OwnerCombatComponent = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	bool IsFly = false;

	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	bool bThirdSkillPressed = false;
};
