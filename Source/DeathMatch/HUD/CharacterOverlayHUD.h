// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterOverlayHUD.generated.h"

/**
 * 
 */
UCLASS()
class DEATHMATCH_API UCharacterOverlayHUD : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HPBar = nullptr;
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* TargetHPBar = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* FirstSkillBar = nullptr;
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* SecondSkillBar = nullptr;
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* ThirdSkillBar = nullptr;


	UPROPERTY(meta = (BindWidget))
	class UTextBlock* AmmoText = nullptr;
};
