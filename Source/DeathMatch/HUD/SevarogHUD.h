#pragma once

#include "CoreMinimal.h"
#include "DeathMatch/HUD/RootHUD.h"
#include "SevarogHUD.generated.h"

/**
 * 
 */
UCLASS()
class DEATHMATCH_API ASevarogHUD : public ARootHUD
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	void SetDashSkillProgressBarHUD(float percent);

private:
	UPROPERTY()
	class USevarogOverlayHUD* SevarogOverlayHUD = nullptr;
};
