#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"

UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem():
	OnCreateSessionDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSession)),
	OnFindSessionsDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessions)),
	OnJoinSessionDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSession)),
	OnDestroySessionDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySession)),
	OnStartSessionDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartSession))
{
	if (IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get())
	{
		SessionInterface = Subsystem->GetSessionInterface();

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Subsystem: %s"), *Subsystem->GetSubsystemName().ToString()));
		}
	}
}

void UMultiplayerSessionsSubsystem::CreateSession(int32 NumPublicConnections, FString MatchType)
{
	if (!SessionInterface.IsValid())
	{
		// Broadcast failure
		MultiplayerOnCreateSessionComplete.Broadcast(false);
		return;
	}

	// If a session already exists, destroy it
	auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession)
	{
		bCreateSessionOnDestroy = true;
		LastNumPublicConnections = NumPublicConnections;
		LastMatchType = MatchType;

		DestroySession();
	}

	// Register callback, storing delegate handle for later removal
	OnCreateSessionHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionDelegate);

	// Create session
	LastSessionSettings = MakeShareable(new FOnlineSessionSettings());
	LastSessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	LastSessionSettings->NumPublicConnections = NumPublicConnections;
	LastSessionSettings->bAllowJoinInProgress = true;
	LastSessionSettings->bUsesPresence = true;
	LastSessionSettings->bAllowJoinViaPresence = true;
	LastSessionSettings->bShouldAdvertise = true;
	LastSessionSettings->bUseLobbiesIfAvailable = true;
	LastSessionSettings->Set(FName("MatchType"), MatchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *LastSessionSettings))
	{
		// Failure occurred. Clear delegate and broadcast to listeners
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionHandle);
		MultiplayerOnCreateSessionComplete.Broadcast(false);
	}
}

void UMultiplayerSessionsSubsystem::FindSessions(int32 MaxSearchResults)
{
	if (!SessionInterface.IsValid())
	{
		// Broadcast failure
		MultiplayerOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		return;
	}

	// Register callback, storing delegate handle for later removal
	OnFindSessionsHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsDelegate);

	// Find sessions
	LastSessionSearch = MakeShareable(new FOnlineSessionSearch());
	LastSessionSearch->MaxSearchResults = MaxSearchResults;
	LastSessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	LastSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), LastSessionSearch.ToSharedRef()))
	{
		// Failure occured. Clear delegate and broadcast to listeners
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsHandle);
		MultiplayerOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
	}
}

void UMultiplayerSessionsSubsystem::JoinSession(const FOnlineSessionSearchResult& SessionResult)
{
	if (!SessionInterface.IsValid())
	{
		// Broadcast failure
		MultiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	}

	// Register callback, storing delegate handle for later removal
	OnJoinSessionHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionDelegate);

	// Join session
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionResult))
	{
		// Failure occurred. Clear delegate and broadcast to listeners
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionHandle);
		MultiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
	}

}

void UMultiplayerSessionsSubsystem::DestroySession()
{
	if (!SessionInterface.IsValid())
	{
		// Broadcast failure
		MultiplayerOnDestroySessionComplete.Broadcast(false);
		return;
	}

	// Register callback, storing delegate handle for later removal
	OnDestroySessionHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionDelegate);

	// Destroy session
	if (!SessionInterface->DestroySession(NAME_GameSession))
	{
		// Failure occurred. Clear delegate and broadcast to listeners
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionHandle);
		MultiplayerOnDestroySessionComplete.Broadcast(false);
	}
}

void UMultiplayerSessionsSubsystem::StartSession()
{
	if (!SessionInterface.IsValid())
	{
		// Broadcast failure
		MultiplayerOnStartSessionComplete.Broadcast(false);
		return;
	}

	// Register callback, storing delegate handle for later removal
	OnStartSessionHandle = SessionInterface->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionDelegate);

	// Start session
	if (!SessionInterface->StartSession(NAME_GameSession))
	{
		// Failure occurred. Clear delegate and broadcast to listeners
		SessionInterface->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionHandle);
		MultiplayerOnStartSessionComplete.Broadcast(false);
	}
}

void UMultiplayerSessionsSubsystem::OnCreateSession(FName SessionName, bool bWasSuccessful)
{
	// Clear delegate handle
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionHandle);
	}
	
	// Broadcast to listeners
	MultiplayerOnCreateSessionComplete.Broadcast(bWasSuccessful);
}

void UMultiplayerSessionsSubsystem::OnFindSessions(bool bWasSuccessful)
{
	// Clear delegate handle
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsHandle);
	}

	// Broadcast to listeners
	if (LastSessionSearch->SearchResults.Num() <= 0)
	{
		MultiplayerOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
	}
	else
	{
		MultiplayerOnFindSessionsComplete.Broadcast(LastSessionSearch->SearchResults, bWasSuccessful);
	}
}

void UMultiplayerSessionsSubsystem::OnJoinSession(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	// Clear delegate handle
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionHandle);
	}

	// Broadcast to listeners
	MultiplayerOnJoinSessionComplete.Broadcast(Result);
}

void UMultiplayerSessionsSubsystem::OnDestroySession(FName SessionName, bool bWasSuccessful)
{
	// Clear delegate handle
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionHandle);
	}

	// Possibly create new session
	if (bWasSuccessful && bCreateSessionOnDestroy)
	{
		bCreateSessionOnDestroy = false;
		CreateSession(LastNumPublicConnections, LastMatchType);
	}

	// Broadcast to listeners
	MultiplayerOnDestroySessionComplete.Broadcast(bWasSuccessful);
}

void UMultiplayerSessionsSubsystem::OnStartSession(FName SessionName, bool bWasSuccessful)
{
	// Clear delegate handle
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionHandle);
	}

	// Broadcast to listeners
	MultiplayerOnStartSessionComplete.Broadcast(bWasSuccessful);
}
