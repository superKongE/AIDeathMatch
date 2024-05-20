// Fill out your copyright notice in the Description page of Project Settings.


#include "DeathMatch/PlayerController/RootPlayerController.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Border.h"

#include "DeathMatch/Character/RootCharacter.h"
#include "DeathMatch/HUD/RootHUD.h"
#include "DeathMatch/HUD/GideonHUD.h"
#include "DeathMatch/CombatComponent/RootCombatComponent.h"
#include "DeathMatch/widget/DeathProgressWidget.h"
#include "DeathMatch/HUD/CharacterOverlayHUD.h"

void ARootPlayerController::BeginPlay()
{
	Super::BeginPlay();

	HUD = Cast<ARootHUD>(GetHUD());
}
void ARootPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}


void ARootPlayerController::CharacterSelect(const int32 index)
{
	OwnerCharacter = OwnerCharacter == nullptr ? Cast<ARootCharacter>(GetCharacter()) : OwnerCharacter;
	if (OwnerCharacter)
	{
		OwnerCharacter->CharacterSelect(index);
	}
}
// ĳ���� ����â���� Ȯ�� ������ ȣ��Ǵ� �Լ�
void ARootPlayerController::CharacterSelectComplete()
{
	OwnerCharacter = OwnerCharacter == nullptr ? Cast<ARootCharacter>(GetCharacter()) : OwnerCharacter;
	if (OwnerCharacter)
	{
		// ����ڰ� ĳ���͸� �����ߴٸ�
		// ĳ���� ������ �Ϸ��� ���� ��ȯ�� ĳ���͸� ���� ĳ���� ��ġ�� ����
		if (OwnerCharacter->CharacterSelectComplete())
		{
			ARootCharacter* PrevOwnerCharacter = OwnerCharacter;
			OwnerCharacter = OwnerCharacter->GetSpawnCharacter();
			OwnerCharacter->SetOwner(this);
			HUD = HUD == nullptr ? Cast<ARootHUD>(GetHUD()) : HUD;
			if (HUD)
				HUD->Reset();
			Possess(OwnerCharacter);

			ClientSetHUD(OwnerCharacter->GetHudClass());
			if (MyHUD)
				HUD = Cast<ARootHUD>(MyHUD);

			OwnerCharacter->SetHP(PrevOwnerCharacter->GetCurrentHP());
			OwnerCharacter->Init();

			PrevOwnerCharacter->DestroyFromCharacterSelect();
			PrevOwnerCharacter->Destroy();
		}
	}
}

void ARootPlayerController::ShowCharacterSelectMenu(bool bShow)
{
	HUD = HUD == nullptr ? Cast<ARootHUD>(GetHUD()) : HUD;
	if (HUD)
	{
		HUD->ShowCharacterSelectMenu(bShow);
	}
}


void ARootPlayerController::SetHUDHP(const float MaxHP, const float TargetHP)
{
	HUD = HUD == nullptr ? Cast<ARootHUD>(GetHUD()) : HUD;
	if (HUD && HUD->GetCharacterOverlay() && HUD->GetCharacterOverlay()->TargetHPBar)
	{
		const float Percent = TargetHP / MaxHP;
		HUD->GetCharacterOverlay()->TargetHPBar->SetPercent(Percent);
	}
}
void ARootPlayerController::SetHUDCurrentHP(const float MaxHP, const float CurrentHP)
{
	HUD = HUD == nullptr ? Cast<ARootHUD>(GetHUD()) : HUD;
	if (HUD && HUD->GetCharacterOverlay() && HUD->GetCharacterOverlay()->HPBar)
	{
		const float Percent = CurrentHP / MaxHP;
		HUD->GetCharacterOverlay()->HPBar->SetPercent(Percent);
	}
}
void ARootPlayerController::SetHUDAmmo(const int32 Ammo, const int32 MaxAmmo)
{
	HUD = HUD == nullptr ? Cast<ARootHUD>(GetHUD()) : HUD;
	if (HUD && HUD->GetCharacterOverlay() && HUD->GetCharacterOverlay()->AmmoText)														
	{
		//HUD->GetCharacterOverlay()->HPBar->SetPercent(0.f);
		FString AmmoText = FString::Printf(TEXT("%d / %d"), Ammo, MaxAmmo);
		HUD->GetCharacterOverlay()->AmmoText->SetText(FText::FromString(AmmoText));
	}
}



void ARootPlayerController::SetDeathProgressText(const float Percent)
{
	HUD = HUD == nullptr ? Cast<ARootHUD>(GetHUD()) : HUD;

	if (HUD && HUD->GetDeathProgress() && HUD->GetDeathProgress()->CountText)
	{
		FString PercentText = FString::Printf(TEXT("%d"), FMath::CeilToInt(Percent));
		HUD->GetDeathProgress()->CountText->SetText(FText::FromString(PercentText));
	}
}
void ARootPlayerController::SetDeathProgressPercent(float Value)
{
	HUD = HUD == nullptr ? Cast<ARootHUD>(GetHUD()) : HUD;
	if (DeathProgessMaterial)
	{
		DeathProgessMaterial->SetScalarParameterValue(TEXT("Percent"), Value);
	}
}
void ARootPlayerController::SetDeathProgress()
{
	HUD = HUD == nullptr ? Cast<ARootHUD>(GetHUD()) : HUD;
	if (HUD && HUD->GetDeathProgress()&& HUD->GetDeathProgress()->DeathProgressBorder)
	{
		DeathProgessMaterial = HUD->GetDeathProgress()->DeathProgressBorder->GetDynamicMaterial();
	}
}
void ARootPlayerController::SetVisibleDeathProgress(const bool bVisible)
{
	HUD = HUD == nullptr ? Cast<ARootHUD>(GetHUD()) : HUD;
	if (HUD && HUD->GetDeathProgress() && HUD->GetDeathProgress()->DeathProgressBorder)
	{
		if (!bVisible)
			HUD->GetDeathProgress()->SetVisibility(ESlateVisibility::Hidden);
		else
			HUD->GetDeathProgress()->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}