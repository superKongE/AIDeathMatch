// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DeathProgressWidget.generated.h"

/**
 * 
 */
UCLASS()
class DEATHMATCH_API UDeathProgressWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CountText = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UBorder* DeathProgressBorder = nullptr;
};
