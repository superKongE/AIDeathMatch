// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ServerRoomSlot.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UServerRoomSlot : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION()
	void ServerRoomSlot_JoinButtonClicked();

public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ServerName = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CurrentPlayers = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* MaxPlayers = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UButton* JoinButton = nullptr;

	int32 ArrayIndex;

};
