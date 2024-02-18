// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DeathMatch/AnimInstance/RootAnimInstance.h"
#include "SevarogAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class DEATHMATCH_API USevarogAnimInstance : public URootAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

private:
	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	class ASevarogCharacter* OwnerCharacter = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	class USevarogCombatComponent* OwnerCombatComponent = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	bool bDash = false;
};
