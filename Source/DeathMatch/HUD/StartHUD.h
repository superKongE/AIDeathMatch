// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "StartHUD.generated.h"

/**
 * 
 */
UCLASS()
class DEATHMATCH_API AStartHUD : public AHUD
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

public:
	void AddStartMenu();

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> StartMenuClass;

	UPROPERTY()
	UUserWidget* StartMenu = nullptr;
};
