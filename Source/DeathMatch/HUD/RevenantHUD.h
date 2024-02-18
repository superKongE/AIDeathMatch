// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DeathMatch/HUD/RootHUD.h"
#include "RevenantHUD.generated.h"

/**
 * 
 */
UCLASS()
class DEATHMATCH_API ARevenantHUD : public ARootHUD
{
	GENERATED_BODY()
	
public:
	ARevenantHUD();
	virtual void BeginPlay() override;

public:
	void AddFirstSkillHUD();

	virtual void ShowSecondSkillHUD(bool bShow);

private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> SecondSkillClass;

	UPROPERTY()
	class UReaper_Teleport_Widget* SecondSkillWidget;
};
