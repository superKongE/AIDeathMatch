#include "DeathMatch/HUD/RootHUD.h"
#include "Engine/Texture2D.h"

#include "Components/ProgressBar.h"

#include "DeathMatch/HUD/CharacterOverlayHUD.h"
#include "DeathMatch/widget/CharacterSelectMenu.h"
#include "DeathMatch/PlayerController/RootPlayerController.h"
#include "DeathMatch/widget/DeathProgressWidget.h"

void ARootHUD::BeginPlay()
{
	Super::BeginPlay();

	AddCharacterOverlay();
	AddCharacterSelectMenu();
	AddDeathProgressWidget();
	ShowCharacterSelectMenu(false);
}
void ARootHUD::Reset()
{
	if (CharacterOverlay != nullptr)
	{
		CharacterOverlay->SetVisibility(ESlateVisibility::Collapsed);
		CharacterOverlay->RemoveFromViewport();
		CharacterOverlay = nullptr;
	}

}

// tick
void ARootHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2D ViewportSize;
	FVector2D ViewportCenter;
	float CrosshairSpread = HUDPackage.CrosshairSpread;

	if (GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		ViewportCenter = FVector2D(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

		if (HUDPackage.CenterCrossHair)
		{
			FVector2D Spread(0.f, 0.f);
			DrawCrossHair(HUDPackage.CenterCrossHair, ViewportCenter, Spread, HUDPackage.CrosshairColor);
		}
		if (HUDPackage.LeftCrossHair)
		{
			FVector2D Spread(-CrosshairSpread, 0.f);
			DrawCrossHair(HUDPackage.LeftCrossHair, ViewportCenter, Spread, HUDPackage.CrosshairColor);
		}
		if (HUDPackage.RightCrossHair)
		{
			FVector2D Spread(CrosshairSpread, 0.f);
			DrawCrossHair(HUDPackage.RightCrossHair, ViewportCenter, Spread, HUDPackage.CrosshairColor);
		}
		if (HUDPackage.UpCrossHair)
		{
			FVector2D Spread(0.f, -CrosshairSpread);
			DrawCrossHair(HUDPackage.UpCrossHair, ViewportCenter, Spread, HUDPackage.CrosshairColor);
		}
		if (HUDPackage.DownCrossHair)
		{
			FVector2D Spread(0.f, CrosshairSpread);
			DrawCrossHair(HUDPackage.DownCrossHair, ViewportCenter, Spread, HUDPackage.CrosshairColor);
		}
	}
}

void ARootHUD::ShowSecondSkillHUD(const bool bShow)
{

}


void ARootHUD::AddCharacterOverlay()
{
	OwningPlayerController = OwningPlayerController == nullptr ? GetOwningPlayerController() : OwningPlayerController;
	if (OwningPlayerController != nullptr && CharacterOverlayClass != nullptr)
	{
		CharacterOverlay = CreateWidget<UCharacterOverlayHUD>(OwningPlayerController, CharacterOverlayClass);
		if (CharacterOverlay != nullptr)
		{
			CharacterOverlay->AddToViewport();
			CharacterOverlay->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
	}
}
void ARootHUD::AddCharacterSelectMenu()
{
	OwningPlayerController = OwningPlayerController == nullptr ? GetOwningPlayerController() : OwningPlayerController;
	if (OwningPlayerController != nullptr && CharacterSelectMenuClass != nullptr)
	{
		CharacterSelectMenu = CreateWidget<UCharacterSelectMenu>(OwningPlayerController, CharacterSelectMenuClass);
		if (CharacterSelectMenu != nullptr)
		{
			CharacterSelectMenu->AddToViewport();
			CharacterSelectMenu->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
	}
}
void ARootHUD::AddDeathProgressWidget()
{
	OwningPlayerController = OwningPlayerController == nullptr ? GetOwningPlayerController() : OwningPlayerController;
	if (OwningPlayerController != nullptr && DeathProgressClass != nullptr)
	{
		DeathProgress = CreateWidget<UDeathProgressWidget>(OwningPlayerController, DeathProgressClass);
		if (DeathProgress != nullptr)
		{
			DeathProgress->AddToViewport();
			DeathProgress->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}
void ARootHUD::SetVisible()
{
	if (CharacterOverlay && CharacterSelectMenu && DeathProgress)
	{
		CharacterOverlay->AddToViewport();
		CharacterOverlay->SetVisibility(ESlateVisibility::HitTestInvisible);
		CharacterSelectMenu->AddToViewport();
		CharacterSelectMenu->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		DeathProgress->AddToViewport();
		DeathProgress->SetVisibility(ESlateVisibility::Collapsed);
	}
}


void ARootHUD::ShowCharacterSelectMenu(const bool bShow)
{
	if (CharacterSelectMenu == nullptr || CharacterOverlay == nullptr) return;

	if (bShow)
	{
		CharacterSelectMenu->SetVisibility(ESlateVisibility::Visible);
		//CharacterOverlay->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

		FInputModeUIOnly InputModeUIOnly;
		OwningPlayerController->SetInputMode(InputModeUIOnly);
		OwningPlayerController->SetShowMouseCursor(true);
	}
	else
	{
		CharacterSelectMenu->SetVisibility(ESlateVisibility::Collapsed);
		//CharacterOverlay->SetVisibility(ESlateVisibility::Hidden);

		FInputModeGameOnly InputModeGameOnly;
		OwningPlayerController->SetInputMode(InputModeGameOnly);
		OwningPlayerController->SetShowMouseCursor(false);
	}
}


void ARootHUD::SetFirstSkillProgressBarHUD(const float percent)
{
	if (CharacterOverlay && CharacterOverlay->FirstSkillBar)
	{
		CharacterOverlay->FirstSkillBar->SetPercent(percent / 100.f);
	}
}
void ARootHUD::SetSecondSkillProgressBarHUD(const float percent)
{
	if (CharacterOverlay && CharacterOverlay->SecondSkillBar)
	{
		CharacterOverlay->SecondSkillBar->SetPercent(percent / 100.f);
	}
}
void ARootHUD::SetThirdSkillProgressBarHUD(const float percent)
{
	if (CharacterOverlay && CharacterOverlay->ThirdSkillBar)
	{
		CharacterOverlay->ThirdSkillBar->SetPercent(percent / 100.f);
	}
}


void ARootHUD::DrawCrossHair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D CrosshairSpread, FLinearColor CrosshairColor)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();
	const FVector2D DrawPoint(
		ViewportCenter.X - (TextureWidth / 2.f) + CrosshairSpread.X,
		ViewportCenter.Y - (TextureHeight / 2.f) + CrosshairSpread.Y
	);

	DrawTexture(
		Texture,
		DrawPoint.X,
		DrawPoint.Y,
		TextureWidth,
		TextureHeight,
		0.f,
		0.f,
		1.f,
		1.f,
		CrosshairColor
	);
}