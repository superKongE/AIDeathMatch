#pragma once

#include "CoreMinimal.h"
#include "DeathMatch/HUD/CharacterOverlayHUD.h"
#include "SevarogOverlayHUD.generated.h"

/**
 * 
 */
UCLASS()
class DEATHMATCH_API USevarogOverlayHUD : public UCharacterOverlayHUD
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* DashSkillBar = nullptr;
};
