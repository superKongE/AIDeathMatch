// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "RootHUD.generated.h"

USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()

public:
	class UTexture2D* LeftCrossHair = nullptr;
	class UTexture2D* RightCrossHair = nullptr;
	class UTexture2D* UpCrossHair = nullptr;
	class UTexture2D* DownCrossHair = nullptr;
	class UTexture2D* CenterCrossHair = nullptr;

	float CrosshairSpread;
	FLinearColor CrosshairColor;
};

UCLASS()
class DEATHMATCH_API ARootHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	void DrawHUD();
	void Reset();
	void SetVisible();

	void DrawCrossHair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D CrosshairSpread, FLinearColor CrosshairColor);

	void AddCharacterOverlay();
	void AddCharacterSelectMenu();
	void AddDeathProgressWidget();

	virtual void ShowSecondSkillHUD(const bool bShow);

	void ShowCharacterSelectMenu(const bool bShow);

	void SetFirstSkillProgressBarHUD(const float percent);
	void SetSecondSkillProgressBarHUD(const float percent);
	void SetThirdSkillProgressBarHUD(const float percent);


public:
	FHUDPackage HUDPackage;

private:
	UPROPERTY()
	class APlayerController* OwningPlayerController = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> CharacterOverlayClass;
	UPROPERTY()
	class UCharacterOverlayHUD* CharacterOverlay = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> DeathProgressClass;
	UPROPERTY()
	class UDeathProgressWidget* DeathProgress = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> CharacterSelectMenuClass;
	UPROPERTY()
	class UCharacterSelectMenu* CharacterSelectMenu = nullptr;

public:
	FORCEINLINE APlayerController* GetOwnerPlayerController() { return OwningPlayerController; }
	FORCEINLINE UCharacterOverlayHUD* GetCharacterOverlay() { return CharacterOverlay; }
	FORCEINLINE UDeathProgressWidget* GetDeathProgress() { return DeathProgress; }
};
