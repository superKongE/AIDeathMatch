#include "DeathMatch/HUD/SevarogHUD.h"
#include "Components/ProgressBar.h"

#include "DeathMatch/HUD/SevarogOverlayHUD.h"

void ASevarogHUD::BeginPlay()
{
	Super::BeginPlay();

	SevarogOverlayHUD = Cast<USevarogOverlayHUD>(GetCharacterOverlay());
}

void ASevarogHUD::SetDashSkillProgressBarHUD(float percent)
{
	if (SevarogOverlayHUD && SevarogOverlayHUD->DashSkillBar)
	{
		SevarogOverlayHUD->DashSkillBar->SetPercent(percent / 100.f);
	}
}