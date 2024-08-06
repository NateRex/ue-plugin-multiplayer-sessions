#include "MultiplayerSessionsMenu.h"
#include "Components/Button.h"
#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"

void UMultiplayerSessionsMenu::Setup(FString LobbyPath, int32 NumberOfPublicConnections, FString TypeOfMatch)
{
	NumPublicConnections = NumberOfPublicConnections;
	MatchType = TypeOfMatch;
	LobbyLevelPath = FString::Printf(TEXT("%s?listen"), *LobbyPath);

	// Add to viewport
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	// Update controller settings
	if (UWorld* World = GetWorld())
	{
		if (APlayerController* PlayerController = World->GetFirstPlayerController())
		{
			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(true);
		}
	}

	// Obtain reference to multiplayer sessions subsystem
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}

	// Bind callbacks to multiplayer sessions subsystem delegates
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
		MultiplayerSessionsSubsystem->MultiplayerOnFindSessionsComplete.AddUObject(this, &ThisClass::OnFindSessions);
		MultiplayerSessionsSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
		MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
		MultiplayerSessionsSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);
	}
}

bool UMultiplayerSessionsMenu::Initialize()
{
	if (!Super::Initialize()) return false;

	if (HostButton)
	{
		HostButton->OnClicked.AddDynamic(this, &ThisClass::OnHostButtonClick);
	}

	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::OnJoinButtonClick);
	}

	return true;
}

void UMultiplayerSessionsMenu::NativeDestruct()
{
	TearDown();
	Super::NativeDestruct();
}

void UMultiplayerSessionsMenu::OnCreateSession(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		// Travel to the lobby level
		if (UWorld* World = GetWorld())
		{
			World->ServerTravel(LobbyLevelPath);
		}
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString("Failed to create session"));
		}

		// Re-enable button
		HostButton->SetIsEnabled(true);
	}
}

void UMultiplayerSessionsMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
	if (!MultiplayerSessionsSubsystem) return;

	for (const FOnlineSessionSearchResult Result : SessionResults)
	{
		// Search for session with same match type
		FString SessionMatchType;
		Result.Session.SessionSettings.Get(FName("MatchType"), SessionMatchType);
		if (SessionMatchType == MatchType)
		{
			MultiplayerSessionsSubsystem->JoinSession(Result);
			return;
		}
	}

	if (!bWasSuccessful || SessionResults.Num() <= 0)
	{
		// Re-enable button
		JoinButton->SetIsEnabled(true);
	}
}

void UMultiplayerSessionsMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	// Resolve IP address and travel to that server
	if (IOnlineSessionPtr SessionInterface = MultiplayerSessionsSubsystem->GetSessionInterface())
	{
		FString Address;
		SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);
		
		if (APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController())
		{
			PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
		}
	}

	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		// Re-enable button
		JoinButton->SetIsEnabled(true);
	}
}

void UMultiplayerSessionsMenu::OnDestroySession(bool bWasSuccessful)
{
}

void UMultiplayerSessionsMenu::OnStartSession(bool bWasSuccessful)
{
}

void UMultiplayerSessionsMenu::OnHostButtonClick()
{
	HostButton->SetIsEnabled(false);
	if (MultiplayerSessionsSubsystem)
	{
		// Create session
		MultiplayerSessionsSubsystem->CreateSession(NumPublicConnections, MatchType);
	}
}

void UMultiplayerSessionsMenu::OnJoinButtonClick()
{
	JoinButton->SetIsEnabled(false);
	if (MultiplayerSessionsSubsystem)
	{
		// Find and join session via a series of callbacks
		MultiplayerSessionsSubsystem->FindSessions(10000);
	}
}

void UMultiplayerSessionsMenu::TearDown()
{
	// Remove from viewport
	RemoveFromParent();

	// Update controller settings
	if (UWorld* World = GetWorld())
	{
		if (APlayerController* PlayerController = World->GetFirstPlayerController())
		{
			FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(false);
		}
	}
}
