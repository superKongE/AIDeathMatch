// Fill out your copyright notice in the Description page of Project Settings.


#include "StartMapHUD.h"
#include "Blueprint/UserWidget.h"
#include "ServerRoomSlot.h"

void AStartMapHUD::DrawHUD()
{
	Super::DrawHUD();
}

void AStartMapHUD::BeginPlay()
{
	Super::BeginPlay();

	if (ServerRoomSlotClass)
	{
		ServerRoomSlot = CreateWidget<UServerRoomSlot>(GetOwningPlayerController(), ServerRoomSlotClass);
	}
}