// Fill out your copyright notice in the Description page of Project Settings.


#include "DeathMatch/HUD/StartHUD.h"
#include "Blueprint/UserWidget.h"

void AStartHUD::BeginPlay()
{
	Super::BeginPlay();

	AddStartMenu();
}

void AStartHUD::AddStartMenu()
{
	if (StartMenuClass)
	{
		APlayerController* OwningPlayerController = GetOwningPlayerController();
		StartMenu = CreateWidget<UUserWidget>(OwningPlayerController, StartMenuClass);

		if (StartMenu)
		{
			StartMenu->AddToViewport();
		}
	}
}