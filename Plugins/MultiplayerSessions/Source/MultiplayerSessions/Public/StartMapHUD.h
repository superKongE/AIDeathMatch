// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "StartMapHUD.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API AStartMapHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;
	virtual void BeginPlay() override;
	
public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> ServerRoomSlotClass;

	class UServerRoomSlot* ServerRoomSlot = nullptr;
};
