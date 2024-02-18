// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RootPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class DEATHMATCH_API ARootPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* InPawn);

public:
	UFUNCTION(BlueprintCallable)
	void CharacterSelect(const int32 index);
	UFUNCTION(BlueprintCallable)
	void CharacterSelectComplete();

	void ShowCharacterSelectMenu(bool bShow);

	void SetHUDHP(const float MaxHP, const float TargetHP);
	void SetHUDCurrentHP(const float MaxHP, const float CurrentHP);
	void SetHUDAmmo(const int32 Ammo, const int32 MaxAmmo);

	void SetDeathProgress();
	void SetVisibleDeathProgress(const bool bVisible);
	void SetDeathProgressText(const float Percent);
	void SetDeathProgressPercent(float Value);

private:
	UPROPERTY()
	TSubclassOf<UUserWidget> ReaperTeleportWidgetClass;
	UPROPERTY()
	UUserWidget* ReaperTeleportWidget = nullptr;

	UPROPERTY()
	class UCharacterOverlayHUD* CharacterOverlay = nullptr;
	UPROPERTY()
	class UDeathProgressWidget* DeathProgress = nullptr;

	UPROPERTY()
	class UMaterialInstanceDynamic* DeathProgessMaterial = nullptr;

	UPROPERTY()
	class ARootHUD* HUD = nullptr;
	UPROPERTY()
	class ARootCharacter* OwnerCharacter = nullptr;

private:
	FORCEINLINE ARootHUD* GetCharacterHUD() { return HUD; }
};
