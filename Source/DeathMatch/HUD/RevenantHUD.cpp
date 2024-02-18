#include "DeathMatch/HUD/RevenantHUD.h"

#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"

#include "DeathMatch/widget/Reaper_Teleport_Widget.h"
#include "DeathMatch/HUD/CharacterOverlayHUD.h"
			
ARevenantHUD::ARevenantHUD()
{
	
}

void ARevenantHUD::BeginPlay()
{
	Super::BeginPlay();

	AddFirstSkillHUD();
}

void ARevenantHUD::AddFirstSkillHUD()
{
	if (SecondSkillClass)
	{
		SecondSkillWidget = CreateWidget<UReaper_Teleport_Widget>(GetOwnerPlayerController(), SecondSkillClass);
		if (SecondSkillWidget)
		{
			SecondSkillWidget->AddToViewport();
			SecondSkillWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void ARevenantHUD::ShowSecondSkillHUD(bool bShow)
{
	if (SecondSkillWidget)
	{
		if (bShow)
			SecondSkillWidget->SetVisibility(ESlateVisibility::Visible);
		else
			SecondSkillWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

