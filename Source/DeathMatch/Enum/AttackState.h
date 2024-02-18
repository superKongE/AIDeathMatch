// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttackState.generated.h"


UENUM(BlueprintType)
enum class EAttackState : uint8 {
	EAS_DefaultAttack UMETA(DisplayName = "DefaultAttack"),
	EAS_StrongAttack UMETA(DisplayName = "StringAttack"),
	EAS_FirstSkill UMETA(DisplayName = "FirstSkill"),
	EAS_SecondSkill UMETA(DisplayName = "SecondSkill"),
	EAS_ThirdSkill UMETA(DisplayName = "ThirdSkill"),
	EAS_FowardDeath UMETA(DisplayName = "FowardDeath"),
	EAS_BackwardDeath UMETA(DisplayName = "BackwardDeath"),
};

/**
 * 
 */
class DEATHMATCH_API AttackState
{
public:
	AttackState();
	~AttackState();
};
