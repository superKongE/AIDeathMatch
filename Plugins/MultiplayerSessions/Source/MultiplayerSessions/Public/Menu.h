// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Menu.generated.h"

/**
 * 
 */

UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void MenuSetup(int32 NumberOfPublicConnections = 4, FString TypeOfMatch = FString(TEXT("FreeForAll")), FString LobbyPath = FString(TEXT("/Game/ThirdPersonCPP/Maps/Lobby")));
	void MenuTearDown();

	UFUNCTION(BlueprintCallable)
	class UMultiplayerSessionsSubsystem* GetUMultiPlayerSessionSubsystem();

protected:

	virtual bool Initialize() override;
	virtual void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld) override;

	//
	// Callbacks for the custom delegates on the MultiplayerSessionsSubsystem
	//
	UFUNCTION()
	void OnCreateSession(bool bWasSuccessful);
	void OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);
	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);
	UFUNCTION()
	void OnStartSession(bool bWasSuccessful);

	UFUNCTION()
	void HostButtonClicked();

	UFUNCTION()
	void RefreshButtonClicked();

	UFUNCTION(BlueprintCallable)
	void ClearServerList();

private:

	UPROPERTY(meta = (BindWidget))
	class UButton* HostButton = nullptr; // 방 생성 버튼

	UPROPERTY(meta = (BindWidget))
	class UButton* FindServer = nullptr; // 참가하기 버튼

	UPROPERTY(meta = (BindWidget))
	class UButton* RefreshButton = nullptr; // 새로고침 버튼

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* ServerNameTextBox = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UScrollBox* ServerList = nullptr;

	UPROPERTY()
	class AStartMapHUD* StartMapHUD = nullptr;


	// The subsystem designed to handle all online session functionality
	class UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

	int32 NumPublicConnections{4};
	FString MatchType{TEXT("FreeForAll")};
	FString PathToLobby{TEXT("")};
};
