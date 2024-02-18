// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerRoomSlot.h"
#include "MultiplayerSessionsSubsystem.h"


void UServerRoomSlot::ServerRoomSlot_JoinButtonClicked()
{
	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
		if (MultiplayerSessionsSubsystem)
		{
			// MultiplayerSessionsSubsystem 의 커스템 델리게이트에 세션 파괴 요청완료시 호출될 콜백함수 바인딩
			MultiplayerSessionsSubsystem->JoinSession(ArrayIndex);
		}
	}
}