// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"
#include "Components/Button.h"
#include "MultiplayerSessionsSubsystem.h"
#include "StartMapHUD.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "ServerRoomSlot.h"

UMultiplayerSessionsSubsystem* UMenu::GetUMultiPlayerSessionSubsystem()
{
	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		UMultiplayerSessionsSubsystem* MultiplayerSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
		return MultiplayerSubsystem;
	}

	return nullptr;
}

// 메뉴 열기
void UMenu::MenuSetup(int32 NumberOfPublicConnections, FString TypeOfMatch, FString LobbyPath)
{
	PathToLobby = FString::Printf(TEXT("%s?listen"), *LobbyPath);
	NumPublicConnections = NumberOfPublicConnections;
	MatchType = TypeOfMatch;
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(true);
		}
	}

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}

	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
		MultiplayerSessionsSubsystem->MultiplayerOnFindSessionsComplete.AddUObject(this, &ThisClass::OnFindSessions);
		MultiplayerSessionsSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
		MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
		MultiplayerSessionsSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);
	}
}
// 메뉴 닫기
void UMenu::MenuTearDown()
{
	RemoveFromParent();
	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(false);
		}
	}
}

bool UMenu::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}
	if (HostButton)
	{
		HostButton->OnClicked.AddDynamic(this, &ThisClass::HostButtonClicked);
	}
	/*if (FindServer)
	{
		FindServer->OnClicked.AddDynamic(this, &ThisClass::RefreshButtonClicked);
	}*/
	if (RefreshButton)
	{
		RefreshButton->OnClicked.AddDynamic(this, &ThisClass::RefreshButtonClicked);
	}

	//StartMapHUD = Cast<AStartMapHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());

	return true;
}

void UMenu::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	MenuTearDown();
	Super::OnLevelRemovedFromWorld(InLevel, InWorld);
}

void UMenu::OnCreateSession(bool bWasSuccessful)
{
	HostButton->SetIsEnabled(true);

	if (bWasSuccessful)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			World->ServerTravel(PathToLobby);
		}
	}
}
void UMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
	RefreshButton->SetIsEnabled(true);

	if (!bWasSuccessful) return;

	if (MultiplayerSessionsSubsystem == nullptr)
	{
		return;
	}
	int32 ArrayIndex = 0;

	// 검색 결과들 중에서 내가 선택한 MatchType과 동일한 Session중에 하나에 Join한다
	for (auto Result : SessionResults)
	{
		if (!Result.IsValid())
		{
			ArrayIndex++;
			continue;
		}

		FString ServerName = "NoName";
		FString SettingsValue;

		Result.Session.SessionSettings.Get(FName("MatchType"), SettingsValue);
		Result.Session.SessionSettings.Get(FName("SERVER_NAME_KEY"), ServerName);

		if (SettingsValue == MatchType)
		{
			FServerInfo Info;
			Info.ServerName = ServerName;
			Info.MaxPlayers = Result.Session.SessionSettings.NumPublicConnections;
			Info.CurrentPlayers = Info.MaxPlayers - Result.Session.NumOpenPublicConnections;
			Info.ServerArrayIndex = ArrayIndex;

			//// ServerList에 서버 목록추가
			if (ServerList)
			{
				if (StartMapHUD && StartMapHUD->ServerRoomSlotClass && StartMapHUD->ServerRoomSlot)
				{
					StartMapHUD->ServerRoomSlot = CreateWidget<UServerRoomSlot>(GetWorld(), StartMapHUD->ServerRoomSlotClass);
					StartMapHUD->ServerRoomSlot->ServerName->SetText(FText::FromString(Info.ServerName));
					StartMapHUD->ServerRoomSlot->CurrentPlayers->SetText(FText::FromString(FString::FromInt(Info.CurrentPlayers)));
					StartMapHUD->ServerRoomSlot->MaxPlayers->SetText(FText::FromString(FString::FromInt(Info.MaxPlayers)));
					StartMapHUD->ServerRoomSlot->ArrayIndex = ArrayIndex;
					StartMapHUD->ServerRoomSlot->JoinButton->OnClicked.AddDynamic(StartMapHUD->ServerRoomSlot, &UServerRoomSlot::ServerRoomSlot_JoinButtonClicked);
					ServerList->AddChild(StartMapHUD->ServerRoomSlot);
				}
			}

			ArrayIndex++;
			//MultiplayerSessionsSubsystem->JoinSession(Result);
			return;
		}
	}
}
void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			FString Address;
			SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);

			APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
			if (PlayerController)
			{
				PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
			}
		}
	}
}
void UMenu::OnDestroySession(bool bWasSuccessful)
{
}
void UMenu::OnStartSession(bool bWasSuccessful)
{
}

void UMenu::ClearServerList()
{
	if (ServerList)
	{
		ServerList->ClearChildren();
	}
}


void UMenu::HostButtonClicked() // 방 생성하기
{
	if (MultiplayerSessionsSubsystem)
	{
		HostButton->SetIsEnabled(false);
		MultiplayerSessionsSubsystem->CreateSession(NumPublicConnections, MatchType, ServerNameTextBox->GetText().ToString());
	}
}
void UMenu::RefreshButtonClicked() // 새로고침 클릭시
{
	// 새로고침 버튼 비활성화 후 모든 목록을 지우고 세션 검색
	// 세션 검색이 완료되면 OnFindSession이 호출된다
	if (MultiplayerSessionsSubsystem)
	{
		// 새로고침 버튼 비활성화
		RefreshButton->SetIsEnabled(false);
		// ServerList의 목록을 모두 지운후
		ServerList->ClearChildren();
		// Session 검색 요청
		MultiplayerSessionsSubsystem->FindSessions(10000);
	}
}
