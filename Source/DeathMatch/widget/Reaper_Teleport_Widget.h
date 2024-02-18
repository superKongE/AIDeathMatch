// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Reaper_Teleport_Widget.generated.h"

/**
 * 
 */
UCLASS()
class DEATHMATCH_API UReaper_Teleport_Widget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	class UImage* LeftClick = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UImage* RightClick = nullptr;
};
